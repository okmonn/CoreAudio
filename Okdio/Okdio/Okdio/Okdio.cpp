#include "Okdio.h"
#include "Loader/Loader.h"
#include "SoundBuffer/SoundBuffer.h"
#include "OkdioList/OkdioList.h"

#include "ReSample/ReSample.h"

// �T�E���h�L���[�ő吔
#define QUEUE_MAX 2

// �R���X�g���N�^
Okdio::Okdio(const std::string& fileName) : 
	read(0), play(false), loop(false), finish(false)
{
	Load(fileName);

	ReSample::Get().Start();
}

// �f�X�g���N�^
Okdio::~Okdio()
{
	ReSample::Get().Finish();

	OkdioList::Get().DeleteList(this);
}

// �ǂݍ���
void Okdio::Load(const std::string& fileName)
{
	auto hr = Loader::Get().Load(fileName);
	if (hr == true)
	{
		name = fileName;
		info = Loader::Get().GetInfo(name);
	}
}

// �Đ�
void Okdio::Play(const bool& loop)
{
	finish     = false;
	play       = true;
	this->loop = loop;
}

// ��~
void Okdio::Stop(void)
{
	play = false;
}

// �g�`�f�[�^���T�E���h�L���[�ɒǉ�
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

// �T�E���h���擾
okmonn::SoundInfo Okdio::GetInfo(void) const
{
	return info;
}

// �T�E���h���Z�b�g
void Okdio::SetInfo(const okmonn::SoundInfo& info)
{
	this->info = info;
}

// �Ō�܂ōĐ��������m�F
bool Okdio::IsFinish(void) const
{
	return finish;
}
