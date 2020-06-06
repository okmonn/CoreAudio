#include "../include/IndexBuffer.h"
#include "../include/Device.h"
#include "Function.h"
#include <cassert>

IndexBuffer::IndexBuffer(const Device& device, const std::uint32_t& buffer_size)
{
    CraeteRsc(device, buffer_size);
}

void IndexBuffer::CraeteRsc(const Device& device, const std::uint32_t& buffer_size)
{
    D3D12_HEAP_PROPERTIES prop{};
    prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.CreationNodeMask     = 0;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
    prop.Type                 = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
    prop.VisibleNodeMask      = 0;

    D3D12_RESOURCE_DESC desc{};
    desc.Alignment        = 0;
    desc.DepthOrArraySize = 1;
    desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
    desc.Format           = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
    desc.Height           = 1;
    desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.MipLevels        = 1;
    desc.SampleDesc       = { 1, 0 };
    desc.Width            = buffer_size;

    auto hr = device.Get()->CreateCommittedResource1(&prop, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, nullptr, IID_PPV_ARGS(GetAddress()));
    assert(hr == S_OK);
}

void IndexBuffer::UpDateIndex(const void* data)
{
    void* buffer = nullptr;
    D3D12_RANGE range{};
    auto hr = Get()->Map(0, &range, &buffer);
    assert(hr == S_OK);

    okmonn::Copy((std::uint8_t*)buffer, data, std::uint32_t(Get()->GetDesc().Width));

    Get()->Unmap(0, &range);
}
