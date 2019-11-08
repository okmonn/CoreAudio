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

// �R���X�g���N�^
Loader::Loader()
{
}

// �f�X�g���N�^
Loader::~Loader()
{
}

std::vector<float> Test(const std::vector<float>& data, const okmonn::SoundInfo& info)
{
	std::vector<float>tmp(data.size() * (double(48000) / double(info.sample)), 0);
	for (size_t i = 0; i < tmp.size(); ++i)
	{
		double integer = 0.0;
		double decimal = std::modf(i * double(info.sample) / double(48000), &integer);
		
		if (info.sample < 48000)
		{
			for (unsigned int n = 0; n < 100; ++n)
			{
				auto sinc = std::sin(std::acos(-1.0) * (n - decimal)) / (std::acos(-1.0) * (n - decimal));
				tmp[i] += data[integer + n] * sinc;
			}
		}
		else
		{
			for (unsigned int n = 0; n < 100; ++n)
			{
				auto sinc = std::sin(std::acos(-1.0) * (n - decimal) * (48000.0 / double(info.sample))) / (std::acos(-1.0) * (n - decimal) * (48000.0 / double(info.sample)));
				tmp[i] += data[integer + n] * sinc;
			}
			tmp[i] /= double(44100) / double(info.sample);
		}
	}

	return tmp;
}

// �ǂݍ���
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

// �폜
bool Loader::Delete(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		sound.erase(sound.find(fileName));
		return true;
	}

	return false;
}

// �T�E���h���擾
okmonn::SoundInfo Loader::GetInfo(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].info;
	}

	return okmonn::SoundInfo();
}

// �T�E���h�f�[�^�擾
std::shared_ptr<std::vector<float>> Loader::GetWave(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave;
	}

	return nullptr;
}

// �T�E���h�f�[�^���擾
size_t Loader::GetWaveNum(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave->size() / sound[fileName].info.byte;
	}

	return 0;
}

// 1��̏����f�[�^��
size_t Loader::ProcessingNum(const std::string& fileName)
{
	return (sound[fileName].info.sample * sound[fileName].info.channel) / 100;
}
