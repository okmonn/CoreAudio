#include "Output.h"
#include "../Window/Window.h"
#include <d3d12.h>

// コンストラクタ
Output::Output(std::weak_ptr<Window>win)
{
	CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, rsc.size(), true);
	CreateRsc(win);
	UAV(0, 0, 0);
}

// デストラクタ
Output::~Output()
{
	for (auto& i : rsc)
	{
		if (i != nullptr)
		{
			i->Release();
			i = nullptr;
		}
	}
	if (heap != nullptr)
	{
		heap->Release();
		heap = nullptr;
	}
}

// リソース生成
void Output::CreateRsc(std::weak_ptr<Window>win)
{
	Vec2 winSize = win.lock()->WinSize();

	D3D12_RESOURCE_DESC desc{};
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height           = winSize.y;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = winSize.x;
	Descriptor::CreateRsc(&rsc[0], DefaultProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE);
}

// リソースの取得
ID3D12Resource* Output::Rsc(void) const
{
	return rsc[0];
}
