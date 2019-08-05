#include "AudioEngin.h"

// コンストラクタ
AudioEngin::AudioEngin() 
{
	for (void*& i : handle)
	{
		i = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	}
}

// デストラクタ
AudioEngin::~AudioEngin()
{
	if (th.joinable() == true)
	{
		SetEvent(handle[1]);
		th.join();
	}
	for (void*& i : handle)
	{
		CloseHandle(i);
	}
}

// 非同期処理
void AudioEngin::Stream(void)
{
	while (true)
	{
		auto hr = WaitForMultipleObjects(handle.size(), handle.data(), false, INFINITE);
		if (hr == WAIT_OBJECT_0)
		{
			break;
		}
	}
}
