#include "Descriptor.h"
#include "../Device/Device.h"
#include <d3d12.h>
#include <crtdbg.h>

// ヒープ生成
void Descriptor::CreateHeap(ID3D12DescriptorHeap** heap, const D3D12_DESCRIPTOR_HEAP_TYPE& type, const unsigned int& rscNum, const bool& shaderFlag)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Flags          = (shaderFlag == true) 
		? D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE 
		: D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NumDescriptors = rscNum;
	desc.Type           = type;
	auto hr = Device::Get().Dev()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&(*heap)));
	_ASSERT(hr == S_OK);
}

// デフォルトヒーププロパティの取得
D3D12_HEAP_PROPERTIES Descriptor::DefaultProp(void)
{
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	prop.Type                 = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;

	return prop;
}

// アップロードヒーププロパティの取得
D3D12_HEAP_PROPERTIES Descriptor::UploadProp(void)
{
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	prop.Type                 = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;

	return prop;
}

// リソースの生成
void Descriptor::CreateRsc(ID3D12Resource** rsc, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state, 
	const D3D12_HEAP_FLAGS& flag, const D3D12_CLEAR_VALUE* clear)
{
	auto hr = Device::Get().Dev()->CreateCommittedResource1(&prop, flag, &desc, state, clear, nullptr, IID_PPV_ARGS(&(*rsc)));
	_ASSERT(hr == S_OK);
}

// コンストラクタ
Descriptor::Descriptor()
{
}

// デストラクタ
Descriptor::~Descriptor()
{
}
