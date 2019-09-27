#include "Okdio.h"
#include "SoundLoader/SoundLoader.h"

// �R���X�g���N�^
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

// �f�X�g���N�^
Okdio::~Okdio()
{
}

// �T�E���h�Q�ƃt�@�C�����擾
std::string Okdio::GetName(void) const
{
	return name;
}

// �T�E���h���擾
okmonn::AudioInfo Okdio::GetInfo(void) const
{
	return SoundLoader::Get().GetInfo(name);
}

// �T�E���h�f�[�^�擾
std::weak_ptr<std::vector<unsigned char>> Okdio::GetWave(void) const
{
	return SoundLoader::Get().GetWave(name);
}

// �T�E���h�f�[�^���擾
size_t Okdio::GetWaveNum(void) const
{
	return SoundLoader::Get().GetWaveNum(name);
}

// �T���v�����O���g���ϊ��p�����[�^�擾
okmonn::ConvertParam Okdio::GetConvertParam(void) const
{
	return SoundLoader::Get().GetConvertParam(name);
}

// �T���v�����O���g���ϊ��p�W���擾
std::weak_ptr<std::vector<double>> Okdio::GetConvertCorre(void) const
{
	return SoundLoader::Get().GetConvertCorre(name);
}
