#include "Constant.h"
#include <d3d12.h>

// コンストラクタ
Constant::Constant(const size_t& size, const size_t& num)
{
	rsc.resize(num);

	CreateRsc(size, num);
}

// デストラクタ
Constant::~Constant()
{
	for (size_t i = 0; i < rsc.size(); ++i)
	{
		if (rsc[i] != nullptr)
		{
			UnMap(i);
			rsc[i]->Release();
			rsc[i] = nullptr;
		}
	}
	if (heap != nullptr)
	{
		heap->Release();
		heap = nullptr;
	}
}

// リソースの生成
void Constant::CreateRsc(const size_t& size, const size_t& num)
{
	D3D12_RESOURCE_DESC desc{};
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	desc.Height           = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = unsigned int(size / num);

	for (size_t i = 0; i < rsc.size(); ++i)
	{
		Descriptor::CreateRsc(&rsc[i], UploadProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ);
	}
}

// リソースの取得
ID3D12Resource* Constant::Rsc(const size_t& index) const
{
	return rsc[index];
}
