#include "Okdio.h"
#include "SoundLoader/SoundLoader.h"

// コンストラクタ
Okdio::Okdio(const std::string& fileName) :
	index(0), play(false), loop(false)
{
	if (SoundLoader::Get().Load(fileName) == 0)
	{
		name   = fileName;
		offset = (SoundLoader::Get().GetConvertCorre(name)->size() - 1) / 2;
		gap    = SoundLoader::Get().GetConvertParam(name).gap;
	}
}

// デストラクタ
Okdio::~Okdio()
{
}

// サウンド参照ファイル名取得
std::string Okdio::GetName(void) const
{
	return name;
}

// サウンド情報取得
okmonn::AudioInfo Okdio::GetInfo(void) const
{
	return SoundLoader::Get().GetInfo(name);
}

// サウンドデータ取得
std::weak_ptr<std::vector<unsigned char>> Okdio::GetWave(void) const
{
	return SoundLoader::Get().GetWave(name);
}

// サウンドデータ数取得
size_t Okdio::GetWaveNum(void) const
{
	return SoundLoader::Get().GetWaveNum(name);
}

// サンプリング周波数変換パラメータ取得
okmonn::ConvertParam Okdio::GetConvertParam(void) const
{
	return SoundLoader::Get().GetConvertParam(name);
}

// サンプリング周波数変換用係数取得
std::weak_ptr<std::vector<double>> Okdio::GetConvertCorre(void) const
{
	return SoundLoader::Get().GetConvertCorre(name);
}
