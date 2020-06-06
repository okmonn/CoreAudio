#include "../include/DepthStencil.h"
#include "../include/Device.h"
#include <cassert>

DepthStencil::DepthStencil()
{
}

DepthStencil::DepthStencil(const Device& device, const okmonn::Vec2& window_size)
{
	CreateDepthStencilView(device, window_size);
}

void DepthStencil::CreateRsc(const Device& device, const okmonn::Vec2& window_size)
{
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.CreationNodeMask     = 0;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	prop.Type                 = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	prop.VisibleNodeMask      = 0;

	D3D12_RESOURCE_DESC desc{};
	desc.Alignment        = 0;
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	desc.Height           = window_size.y;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = window_size.x;

	D3D12_CLEAR_VALUE clear{};
	clear.DepthStencil.Depth   = 1.0f;
	clear.DepthStencil.Stencil = 0;
	clear.Format               = desc.Format;

	auto hr = device.Get()->CreateCommittedResource1(&prop, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear, nullptr, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}

void DepthStencil::CreateDepthStencilView(const Device& device, const okmonn::Vec2& window_size)
{
	heap.CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	CreateRsc(device, window_size);

	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Flags              = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
	desc.Format             = Get()->GetDesc().Format;
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension      = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;

	device.Get()->CreateDepthStencilView(Get(), &desc, heap.Get()->GetCPUDescriptorHandleForHeapStart());
}
