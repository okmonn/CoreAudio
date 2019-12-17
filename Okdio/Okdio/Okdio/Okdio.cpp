#include "Okdio.h"
#include "Loader/Loader.h"
#include "SoundBuffer/SoundBuffer.h"
#include "OkdioList/OkdioList.h"

#include "ReSample/ReSample.h"

// サウンドキュー最大数
#define QUEUE_MAX 2

// コンストラクタ
Okdio::Okdio(const std::string& fileName) : 
	read(0), play(false), loop(false), finish(false)
{
	Load(fileName);

	ReSample::Get().Start();
}

// デストラクタ
Okdio::~Okdio()
{
	ReSample::Get().Finish();

	OkdioList::Get().DeleteList(this);
}

// 読み込み
void Okdio::Load(const std::string& fileName)
{
	auto hr = Loader::Get().Load(fileName);
	if (hr == true)
	{
		name = fileName;
		info = Loader::Get().GetInfo(name);
	}
}

// 再生
void Okdio::Play(const bool& loop)
{
	finish     = false;
	play       = true;
	this->loop = loop;
}

// 停止
void Okdio::Stop(void)
{
	play = false;
}

// 波形データをサウンドキューに追加
void Okdio::Submit(const size_t& num)
{
	if (play == true)
	{
		std::weak_ptr<std::vector<float>>wave = Loader::Get().GetWave(name);
		size_t size = num * (float(info.sample) / 48000.0f);
		if (read + size > wave.lock()->size())
		{
			size = (read + size) - wave.lock()->size();

			auto tmp = ReSample::Get().Convert(std::vector<float>(&wave.lock()->at(read), &wave.lock()->at(read + size)), info, okmonn::SoundInfo(48000, 32, 2));

			SoundBuffer::Get().Submit(tmp);
			if (loop == false)
			{
				Stop();
				finish = true;
			}

			read = 0;
		}
		else
		{
			auto tmp = ReSample::Get().Convert(std::vector<float>(&wave.lock()->at(read), &wave.lock()->at(read + size)), info, okmonn::SoundInfo(48000, 32, 2));

			SoundBuffer::Get().Submit(tmp);
			read += size;
		}
	}
}

// サウンド情報取得
okmonn::SoundInfo Okdio::GetInfo(void) const
{
	return info;
}

// サウンド情報セット
void Okdio::SetInfo(const okmonn::SoundInfo& info)
{
	this->info = info;
}

// 最後まで再生したか確認
bool Okdio::IsFinish(void) const
{
	return finish;
}
