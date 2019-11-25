#include "Descriptor.h"
#include "../Device/Device.h"
#include <d3d12.h>
#include <crtdbg.h>

// コンストラクタ
Descriptor::Descriptor() : 
	heap(nullptr), buf(nullptr)
{
	rsc.resize(1);
}

// デストラクタ
Descriptor::~Descriptor()
{
}

// ヒープの生成
void Descriptor::CreateHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& type, const unsigned int& rscNum, const bool& shaderFlag)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Flags = (shaderFlag == true)
		? D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		: D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NumDescriptors = rscNum;
	desc.Type           = type;
	auto hr = Device::Get().Dev()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	_ASSERT(hr == S_OK);
}

// リソースの生成
void Descriptor::CreateRsc(ID3D12Resource** rsc, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state,
	const D3D12_HEAP_FLAGS& flag, const D3D12_CLEAR_VALUE* clear)
{
	auto hr = Device::Get().Dev()->CreateCommittedResource1(&prop, flag, &desc, state, clear, nullptr, IID_PPV_ARGS(&(*rsc)));
	_ASSERT(hr == S_OK);
}

// RTVの生成
void Descriptor::RTV(const unsigned int& index)
{
	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format        = rsc[index]->GetDesc().Format;
	desc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;

	auto handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += Device::Get().Dev()->GetDescriptorHandleIncrementSize(heap->GetDesc().Type) * index;
	Device::Get().Dev()->CreateRenderTargetView(rsc[index], &desc, handle);
}

// UAVの生成
void Descriptor::UAV(const unsigned int& index, const unsigned int& num, const unsigned int& stride)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.Buffer.NumElements         = num;
	desc.Buffer.StructureByteStride = stride;
	desc.Format                     = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension              = (num == 0) 
		? D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER
		: D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D;

	auto handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += Device::Get().Dev()->GetDescriptorHandleIncrementSize(heap->GetDesc().Type) * index;
	Device::Get().Dev()->CreateUnorderedAccessView(rsc[index], nullptr, &desc, handle);
}

// マップ
void Descriptor::Map(const unsigned int& index, void* data)
{
	auto hr = rsc[index]->Map(0, nullptr, &buf);
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

// ヒープの取得
ID3D12DescriptorHeap* Descriptor::Heap(void) const
{
	return heap;
}
