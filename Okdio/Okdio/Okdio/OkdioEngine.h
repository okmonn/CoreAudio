#pragma once
#include "ClassBase/UnKnown.h"
#include "Information.h"
#include "Function/Function.h"
#include <list>
#include <wrl.h>
#include <array>
#include <mutex>
#include <vector>
#include <thread>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
class Okdio;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
	OkdioEngine : public UnKnown<OkdioEngine>
{
public:
	// コンストラクタ
	OkdioEngine(const okmonn::AudioDeviceType& devType, const okmonn::AudioType& audioType, const char& deviceIndex = -1);
	// デストラクタ
	~OkdioEngine();

	// Okdio生成
	long CreateOkdio(const std::string& fileName, const GUID& id, void** obj);

	// 開始
	void Start(void);

	// 停止
	void Stop(void);

private:
	void operator=(const OkdioEngine&) = delete;

	// オーディオデバイス生成
	void CreateDevice(const char& deviceIndex);

	// フォーマット設定
	void* SetFormat(IAudioClient* audio, const okmonn::AudioInfo* info = nullptr);

	// オーディオクライアント生成
	void CreateAudioClient(void);

	// オーディオクライアント初期化
	void Initialize(void);

	// オーディオレンダラー生成
	void CreateAudioRender(void);

	// 非同期処理
	void Stream(void);

	void TTT(std::vector<float>& data);


	// オーディオデバイス
	Microsoft::WRL::ComPtr<IMMDevice>device;

	// オーディオクライアント
	Microsoft::WRL::ComPtr<IAudioClient>audio;

	// オーディオレンダラー
	Microsoft::WRL::ComPtr<IAudioRenderClient>render;

	// オーディオデバイスタイプ
	okmonn::AudioDeviceType devType;

	// オーディオタイプ
	okmonn::AudioType audioType;

	// オーディオ情報
	okmonn::AudioInfo info;

	// 排他制御
	std::mutex mtx;

	// スレッド
	std::thread th;

	// イベントハンドル
	std::array<void*, 2>handle;

	// Okdioリスト
	std::list<Okdio*>okdio;
};
