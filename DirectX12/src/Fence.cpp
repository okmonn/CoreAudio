#include "../include/Fence.h"
#include "../include/Device.h"
#include "../include/CommandQueue.h"
#include <cassert>

Fence::Fence(const Device& device, const CommandQueue& command_queue) : 
	command_queue(&command_queue), count(0)
{
	CreateFence(device);
}

void Fence::CreateFence(const Device& device)
{
	auto hr = device.Get()->CreateFence(count, 
		D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}

void Fence::WaitForCommandExecution(void)
{
	auto hr = command_queue->Get()->Signal(Get(), ++count);
	assert(hr == S_OK);

	if (Get()->GetCompletedValue() != count) {
		void* handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		assert(handle != nullptr);

		hr = Get()->SetEventOnCompletion(count, handle);
		assert(hr == S_OK);

		WaitForSingleObjectEx(handle, INFINITE, false);

		CloseHandle(handle);
	}
}
