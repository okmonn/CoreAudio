#include "Loader.h"
#include "../Format/WavFmt.h"
#include <functional>
#include "../Function/Function.h"

namespace
{
	std::unordered_map<std::string, std::function<bool(const std::string&, std::shared_ptr<std::vector<float>>&, okmonn::SoundInfo&)>>func = {
		{"wav", std::bind(&wav::Load, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)}
	};
}

// コンストラクタ
Loader::Loader()
{
}

// デストラクタ
Loader::~Loader()
{
}

bool Loader::Load(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return true;
	}

	std::string fmt = fileName.substr(fileName.find_last_of('.') + 1);
	if (func.find(fmt) != func.end())
	{
		return func[fmt](fileName, sound[fileName].wave, sound[fileName].info);
	}

	return false;
}

// 削除
bool Loader::Delete(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		sound.erase(sound.find(fileName));
		return true;
	}

	return false;
}

// サウンド情報取得
okmonn::SoundInfo Loader::GetInfo(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].info;
	}

	return okmonn::SoundInfo();
}

// サウンドデータ取得
std::shared_ptr<std::vector<float>> Loader::GetWave(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave;
	}

	return nullptr;
}

// サウンドデータ数取得
size_t Loader::GetWaveNum(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave->size() / sound[fileName].info.byte;
	}

	return 0;
}

// 1回の処理データ量
size_t Loader::ProcessingNum(const std::string& fileName)
{
	return (sound[fileName].info.sample * sound[fileName].info.channel) / 100;
}
