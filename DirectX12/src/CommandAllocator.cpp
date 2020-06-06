#include "../include/CommandAllocator.h"
#include "../include/Device.h"
#include <assert.h>

CommandAllocator::CommandAllocator(const Device& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	CreateCommandAllocator(device, type);
}

void CommandAllocator::CreateCommandAllocator(const Device& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	auto hr = device.Get()->CreateCommandAllocator(type, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}

void CommandAllocator::Reset(void) const
{
	auto hr = Get()->Reset();
	assert(hr == S_OK);
}
