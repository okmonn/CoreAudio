#include "../include/ResourceBuffer.h"
#include "../include/Device.h"
#include <cassert>

ResourceBuffer::ResourceBuffer()
{
}

ResourceBuffer::ResourceBuffer(const Device& device, const std::uint32_t& width, const std::uint32_t& height)
{
	CreateShaderResourceView(device, width, height);
}

void ResourceBuffer::CreateRsc(const Device& device, const std::uint32_t& width, const std::uint32_t& height)
{
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask     = 0;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	prop.Type                 = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask      = 0;

	D3D12_RESOURCE_DESC desc{};
	desc.Alignment        = 0;
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height           = height;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = width;

	auto hr = device.Get()->CreateCommittedResource1(&prop, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}

void ResourceBuffer::CreateShaderResourceView(const Device& device, const std::uint32_t& width, const std::uint32_t& height)
{
	heap.CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);
	CreateRsc(device, width, height);

	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format                  = Get()->GetDesc().Format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels     = Get()->GetDesc().MipLevels;
	desc.ViewDimension           = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;

	device.Get()->CreateShaderResourceView(Get(), &desc, heap.Get()->GetCPUDescriptorHandleForHeapStart());
}

void ResourceBuffer::UpDateResource(const void* data)
{
	D3D12_BOX box{};
	box.back   = 1;
	box.bottom = Get()->GetDesc().Height;
	box.front  = 0U;
	box.left   = 0;
	box.right  = std::uint32_t(Get()->GetDesc().Width);
	box.top    = 0;

	auto hr = Get()->WriteToSubresource(0U, &box, data, box.right * 4, box.bottom * 4);
	assert(hr == S_OK);
}
