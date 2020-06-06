#include "../include/CommandQueue.h"
#include "../include/Device.h"
#include "../include/CommandList.h"
#include <cassert>

CommandQueue::CommandQueue(const Device& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	CreateCommandQueue(device, type);
}

void CommandQueue::CreateCommandQueue(const Device& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Flags    = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type     = type;

	auto hr = device.Get()->CreateCommandQueue(&desc, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}

void CommandQueue::ExecutionCommand(const std::vector<CommandList*>& command_list)
{
	std::vector<ID3D12CommandList*>list;
	for (auto& i : command_list) {
		list.push_back(i->Get());
	}
	Get()->ExecuteCommandLists(std::uint32_t(list.size()), list.data());
}
