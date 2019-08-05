#pragma once
#include "../etc/Unknown.h"
#include <array>
#include <thread>

class Manager;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
AudioEngin : public MyUnknown<AudioEngin>
{
	friend Manager;
public:

private:
	// コンストラクタ
	AudioEngin();
	// デストラクタ
	~AudioEngin();

	// 非同期処理
	void Stream(void);


	// イベント
	std::array<void*, 2>handle;

	// スレッド
	std::thread th;
};
