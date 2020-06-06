#ifndef DIRECTX_COMMAND_ALLOCATOR
#define DIRECTX_COMMAND_ALLOCATOR

#include "BaseClass.h"

struct Device;

struct CommandAllocator : 
	public BaseClass<ID3D12CommandAllocator> {
public:
	/*コンストラクタ
	.Device
	.コマンドタイプ*/
	CommandAllocator(const Device& device, const D3D12_COMMAND_LIST_TYPE& type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);

public:
	/*リセット*/
	void Reset(void) const;

private:
	/*コマンドアロケータの生成
	.Device
	.コマンドタイプ*/
	void CreateCommandAllocator(const Device& device, const D3D12_COMMAND_LIST_TYPE& type);
};

#endif
