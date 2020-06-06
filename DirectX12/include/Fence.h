#ifndef DIRECTX_FENCE
#define DIRECTX_FENCE

#include "BaseClass.h"
#include <cstdint>

struct Device;
struct CommandQueue;

struct Fence :
	public BaseClass<ID3D12Fence1> {
public:
	/*コンストラクタ
	.Device
	.CommandQueue*/
	Fence(const Device& device, const CommandQueue& command_queue);

public:
	/*コマンドリストの待機*/
	void WaitForCommandExecution(void);

private:
	/*フェンスの生成
	.Device*/
	void CreateFence(const Device& device);

private:
	/*CommandQueue*/
	const CommandQueue* command_queue;

private:
	/*フェンスカウント*/
	std::uint64_t count;
};

#endif
