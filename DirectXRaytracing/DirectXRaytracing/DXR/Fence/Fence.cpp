#include "Fence.h"
#include "../Device/Device.h"
#include "../Queue/Queue.h"
#include <d3d12.h>
#include <crtdbg.h>

// コンストラクタ
Fence::Fence(std::weak_ptr<Queue>queue) : queue(queue),
	fence(nullptr), fenceCnt(0), eventHandle(CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS))
{
	CreateFence();
}

// デストラクタ
Fence::~Fence()
{
	if (fence != nullptr)
	{
		fence->Release();
		fence = nullptr;
	}
	if (eventHandle != nullptr)
	{
		CloseHandle(eventHandle);
		eventHandle = nullptr;
	}
}

// フェンスの生成
void Fence::CreateFence(void)
{
	auto hr = Device::Get().Dev()->CreateFence(fenceCnt, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	_ASSERT(hr == S_OK);
}

// コマンド待機
void Fence::Wait(void)
{
	queue.lock()->Signal(fence, fenceCnt);

	if (fence->GetCompletedValue() != fenceCnt)
	{
		auto hr = fence->SetEventOnCompletion(fenceCnt, eventHandle);
		_ASSERT(hr == S_OK);

		WaitForSingleObject(eventHandle, INFINITE);
	}
}

// フェンスの取得
ID3D12Fence1* Fence::Fen(void) const
{
	return fence;
}

