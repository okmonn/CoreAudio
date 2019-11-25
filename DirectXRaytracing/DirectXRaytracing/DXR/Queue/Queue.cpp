#include "Queue.h"
#include "../Device/Device.h"
#include <d3d12.h>
#include <crtdbg.h>

// コンストラクタ
Queue::Queue(const D3D12_COMMAND_LIST_TYPE& type) :
	queue(nullptr)
{
	CreateQueue(type);
}

// デストラクタ
Queue::~Queue()
{
	if (queue != nullptr)
	{
		queue->Release();
		queue = nullptr;
	}
}

// コマンドキューの生成
void Queue::CreateQueue(const D3D12_COMMAND_LIST_TYPE& type)
{
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type  = type;
	auto hr = Device::Get().Dev()->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
	_ASSERT(hr == S_OK);
}

// フェンスカウントのセット
void Queue::Signal(ID3D12Fence1* fence, unsigned __int64& fenceCnt)
{
	auto hr = queue->Signal(fence, ++fenceCnt);
	_ASSERT(hr == S_OK);
}

// コマンドキューの取得
ID3D12CommandQueue* Queue::Que(void) const
{
	return queue;
}
