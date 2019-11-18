#pragma once
#include "Base/Unknown.h"
#include "Function/Function.h"
#include <list>
#include <wrl.h>
#include <array>
#include <mutex>
#include <thread>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
class Okdio;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
	OkdioEngine : 
	public okmonn::Unknown<OkdioEngine>
{
public:
	// コンストラクタ
	OkdioEngine(const okmonn::AudioDevType& devType, const okmonn::AudioType& audioType, const int& devIndex);
	// デストラクタ
	~OkdioEngine();

	// Okdio生成
	bool CreateOkdio(const std::string& fileName, const GUID& id, void** okdio);

	// 開始
	bool Start(void);

	// 停止
	bool Stop(void);

private:
	// オーディオデバイス生成
	void CreateDevice(const int& devIndex);

	// オーディオクライアント生成
	void CreateAudio(void);

	// フォーマット取得
	void* GetFormat(void);

	// オーディオクライアント初期化
	void Initialized(void);

	// オーディオレンダラー生成
	void CreateRender(void);

	// 非同期処理
	void Stream(void);


	// オーディオデバイスタイプ
	okmonn::AudioDevType devType;

	// オーディオタイプ
	okmonn::AudioType audioType;

	// サウンド情報
	okmonn::SoundInfo info;

	// 排他制御
	std::mutex mtx;

	// スレッド
	std::thread th;

	// オーディオデバイス
	Microsoft::WRL::ComPtr<IMMDevice>dev;

	// オーディオクライアント
	Microsoft::WRL::ComPtr<IAudioClient>audio;

	// オーディオレンダラー
	Microsoft::WRL::ComPtr<IAudioRenderClient>render;

	// イベントハンドル
	std::array<void*, 2>handle;
};
