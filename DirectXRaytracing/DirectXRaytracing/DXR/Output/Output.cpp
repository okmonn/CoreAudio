#include "Output.h"
#include "../Device/Device.h"
#include "../Window/Window.h"
#include "../Acceleration/Acceleration.h"
#include <d3d12.h>

// コンストラクタ
Output::Output(std::weak_ptr<Window>win, std::weak_ptr<Acceleration>top)
{
	CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
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

// SRVの生成
void Output::SRV(std::weak_ptr<Acceleration> top)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.RaytracingAccelerationStructure.Location = top.lock()->Result()->GetGPUVirtualAddress();
	desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension                            = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;

	auto handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += Device::Get().Dev()->GetDescriptorHandleIncrementSize(heap->GetDesc().Type);
	Device::Get().Dev()->CreateShaderResourceView(nullptr, &desc, handle);
}

// リソースの取得
ID3D12Resource* Output::Rsc(void) const
{
	return *rsc.begin();
}
