#pragma once
#include "../Base/Singleton.h"
#include <mutex>
#include <vector>
#include <memory>

class SoundBuffer : 
	public okmonn::Singleton<SoundBuffer>
{
	friend okmonn::Singleton<SoundBuffer>;

public:
	// バッファのサイズ設定
	void ReSize(const size_t& num);

	// バッファに追加
	void Submit(const std::vector<float>& wave);

	// バッファクリア
	void Clear(void);

	// サウンドデータ取得
	std::weak_ptr<std::vector<float>> GetWave(void) const;

private:
	// コンストラクタ
	SoundBuffer();
	// デストラクタ
	~SoundBuffer();


	// 排他制御
	std::mutex mtx;

	// サウンドデータバッファ
	std::shared_ptr<std::vector<float>>wave;
};
