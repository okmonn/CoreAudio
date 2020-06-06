#ifndef DIRECTX_COMMAND_QUEUE
#define DIRECTX_COMMAND_QUEUE

#include "BaseClass.h"
#include <vector>

struct Device;
struct CommandList;

struct CommandQueue :
	public BaseClass<ID3D12CommandQueue> {
public:
	/*コンストラクタ
	.Device
	.コマンドタイプ*/
	CommandQueue(const Device& device, const D3D12_COMMAND_LIST_TYPE& type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);

public:
	/*コマンドの実行
	.CommandListの配列*/
	void ExecutionCommand(const std::vector<CommandList*>& command_list);

private:
	/*コマンドキューの生成
	.Device
	.コマンドタイプ*/
	void CreateCommandQueue(const Device& device, const D3D12_COMMAND_LIST_TYPE& type);
};

#endif
