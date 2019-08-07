#pragma once
#include "../etc/Unknown.h"
#include "../etc/Info.h"
#include <wrl.h>
#include <array>
#include <vector>
#include <memory>
#include <thread>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
class Manager;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
AudioEngin : public MyUnknown<AudioEngin>
{
	friend Manager;
public:
	// 初期化
	long Initialize(const okmonn::AudioType& type, const okmonn::Info* info = nullptr);

	// エンジン開始
	long Start(void);

	// エンジン停止
	long Stop(void);

private:
	// コンストラクタ
	AudioEngin(const unsigned int& index, const okmonn::DeviceType& type);
	// デストラクタ
	~AudioEngin();

	// エンドポイントデバイス生成
	void CreateDevice(const unsigned int& index, const okmonn::DeviceType& type);

	// オーディオクライアント生成
	long CreateAudio(const okmonn::AudioType& type, void* addr);

	// オーディオレンダラー生成
	long CreateRender(void);

	// 非同期処理
	void Stream(void);


	// エンドポイントデバイス
	Microsoft::WRL::ComPtr<IMMDevice>device;

	// オーディオクライアント
	Microsoft::WRL::ComPtr<IAudioClient>audio;

	// オーディオレンダラー
	IAudioRenderClient* render;

	// サウンド情報
	okmonn::Info info;

	// エンドポイントデバイスタイプ
	okmonn::DeviceType deviceType;

	// オーディオタイプ
	okmonn::AudioType audioType;

	// イベント
	std::array<void*, 2>handle;

	// スレッド
	std::thread th;

	std::shared_ptr<std::vector<unsigned char>>wave;
	okmonn::Info waveInfo;
};
