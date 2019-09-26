#include "SoundLoader.h"
#include "../Format/WavFormat.h"
#include "../Function/Function.h"
#include <functional>

namespace
{
	std::unordered_map<std::string, std::function<int(const std::string&, okmonn::AudioInfo&, std::shared_ptr<std::vector<unsigned char>>&)>>func = {
		{"wav", std::bind(&wav::Load, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)}
	};
}

// �R���X�g���N�^
SoundLoader::SoundLoader()
{
}

// �f�X�g���N�^
SoundLoader::~SoundLoader()
{
}

// �o�̓T�E���h���Z�b�g
void SoundLoader::SetOutInfo(const okmonn::AudioInfo& info)
{
	this->info = info;
}

// �ǂݍ���
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
			sound[fileName].param = okmonn::GetConvertParam(info.sample, sound[fileName].info.sample);
			sound[fileName].corre = std::make_shared<std::vector<double>>(okmonn::Sinc(100, okmonn::GetDegree(100, sound[fileName].param), sound[fileName].param));
		}

		return hr;
	}

	return -1;
}

// �w��f�[�^���폜
void SoundLoader::Delete(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		sound.erase(fileName);
	}
}

// �T�E���h���擾
okmonn::AudioInfo SoundLoader::GetInfo(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].info;
	}

	return okmonn::AudioInfo();
}

// �T�E���h�f�[�^�擾
std::shared_ptr<std::vector<unsigned char>> SoundLoader::GetWave(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave;
	}

	return nullptr;
}

// �T�E���h�f�[�^���擾
size_t SoundLoader::GetWaveNum(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].wave->size() / (sound[fileName].info.bit / 8);
	}

	return 0;
}

// �T���v�����O���g���ϊ��p�����[�^�擾
okmonn::ConvertParam SoundLoader::GetConvertParam(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].param;
	}

	return okmonn::ConvertParam();
}

// �T���v�����O���g���ϊ��p�W���擾
std::shared_ptr<std::vector<double>> SoundLoader::GetConvertCorre(const std::string& fileName)
{
	if (sound.find(fileName) != sound.end())
	{
		return sound[fileName].corre;
	}

	return nullptr;
}
