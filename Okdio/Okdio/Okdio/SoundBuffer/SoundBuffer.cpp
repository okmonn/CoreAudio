#include "SoundBuffer.h"
#include <algorithm>

// �R���X�g���N�^
SoundBuffer::SoundBuffer()
{
	wave = std::make_shared<std::vector<float>>();
}

// �f�X�g���N�^
SoundBuffer::~SoundBuffer()
{
}

// �o�b�t�@�̃T�C�Y�ݒ�
void SoundBuffer::ReSize(const size_t& num)
{
	wave->resize(num);
}

// �o�b�t�@�ɒǉ�
void SoundBuffer::Submit(const std::vector<float>& wave)
{
	std::unique_lock<std::mutex>lick(mtx);
	std::transform(wave.begin(), wave.end(), this->wave->begin(), this->wave->begin(), std::plus<float>());
}

// �o�b�t�@�N���A
void SoundBuffer::Clear(void)
{
	wave->clear();
}

// �T�E���h�f�[�^�擾
std::weak_ptr<std::vector<float>> SoundBuffer::GetWave(void) const
{
	return wave;
}
