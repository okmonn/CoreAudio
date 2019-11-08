#include "SoundBuffer.h"
#include <algorithm>

// コンストラクタ
SoundBuffer::SoundBuffer()
{
	wave = std::make_shared<std::vector<float>>();
}

// デストラクタ
SoundBuffer::~SoundBuffer()
{
}

// バッファのサイズ設定
void SoundBuffer::ReSize(const size_t& num)
{
	wave->resize(num);
}

// バッファに追加
void SoundBuffer::Submit(const std::vector<float>& wave)
{
	std::unique_lock<std::mutex>lick(mtx);
	std::transform(wave.begin(), wave.end(), this->wave->begin(), this->wave->begin(), std::plus<float>());
}

// バッファクリア
void SoundBuffer::Clear(void)
{
	wave->clear();
}

// サウンドデータ取得
std::weak_ptr<std::vector<float>> SoundBuffer::GetWave(void) const
{
	return wave;
}
