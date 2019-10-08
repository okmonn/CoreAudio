#include "SoundLoader.h"
#include "../Format/WavFormat.h"
#include <functional>

namespace
{
	std::unordered_map<std::string, std::function<int(const std::string&, okmonn::AudioInfo&, std::shared_ptr<std::vector<float>>&)>>func = {
		{"wav", std::bind(&wav::Load, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)}
	};
}

// コンストラクタ
SoundLoader::SoundLoader()
{
}

// デストラクタ
SoundLoader::~SoundLoader()
{
}

// 出力サウンド情報セット
void SoundLoader::SetOutInfo(const okmonn::AudioInfo& info)
{
	this->info = info;
}

// サンプリング周波数変換用係数を求める
void SoundLoader::ConvertCorre(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		if (convert.find(sound[fileName].info.sample) == convert.end())
		{
			convert[sound[fileName].info.sample].param = okmonn::GetConvertParam(info.sample, sound[fileName].info.sample);
			convert[sound[fileName].info.sample].corre = std::make_shared<std::vector<double>>(okmonn::Sinc(100, okmonn::GetDegree(100, convert[sound[fileName].info.sample].param), convert[sound[fileName].info.sample].param));
		}
	}
}

// 読み込み
int SoundLoader::Load(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return 0;
	}

	std::string fmt = fileName.substr(fileName.find_last_of('.') + 1);
	if (func.find(fmt) != func.end())
	{
		auto hr = func[fmt](fileName, sound[fileName].info, sound[fileName].wave);
		if (hr == 0)
		{
			ConvertCorre(fileName);
		}

		return hr;
	}

	return -1;
}

// 指定データを削除
void SoundLoader::Delete(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		sound.erase(fileName);
	}
}

// サウンド情報取得
okmonn::AudioInfo SoundLoader::GetInfo(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].info;
	}

	return okmonn::AudioInfo();
}

// サウンドデータ取得
std::shared_ptr<std::vector<float>> SoundLoader::GetWave(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave;
	}

	return nullptr;
}

// サウンドデータ数取得
size_t SoundLoader::GetWaveNum(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave->size() / (sound[fileName].info.byte);
	}

	return 0;
}

// サンプリング周波数変換パラメータ取得
okmonn::ConvertParam SoundLoader::GetConvertParam(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return convert[sound[fileName].info.sample].param;
	}

	return okmonn::ConvertParam();
}

// サンプリング周波数変換用係数取得
std::shared_ptr<std::vector<double>> SoundLoader::GetConvertCorre(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return convert[sound[fileName].info.sample].corre;
	}

	return nullptr;
}
