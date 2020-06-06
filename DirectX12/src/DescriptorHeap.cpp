#include "../include/DescriptorHeap.h"
#include "../include/Device.h"
#include "../include/CommandList.h"
#include <cassert>

DescriptorHeap::DescriptorHeap()
{
}

void DescriptorHeap::CreateHeap(const Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE& type, const std::uint32_t& rsc_num, const bool& shader_use_flag)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	if (shader_use_flag == true) {
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}
	else {
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;;
	}
	desc.NodeMask       = 0;
	desc.NumDescriptors = rsc_num;
	desc.Type           = type;

	auto hr = device.Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}
