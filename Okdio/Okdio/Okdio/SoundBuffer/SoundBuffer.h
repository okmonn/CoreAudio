#pragma once
#include "../Base/Singleton.h"
#include <mutex>
#include <vector>
#include <memory>

class SoundBuffer : 
	public okmonn::Singleton<SoundBuffer>
{
	friend okmonn::Singleton<SoundBuffer>;

public:
	// �o�b�t�@�̃T�C�Y�ݒ�
	void ReSize(const size_t& num);

	// �o�b�t�@�ɒǉ�
	void Submit(const std::vector<float>& wave);

	// �o�b�t�@�N���A
	void Clear(void);

	// �T�E���h�f�[�^�擾
	std::weak_ptr<std::vector<float>> GetWave(void) const;

private:
	// �R���X�g���N�^
	SoundBuffer();
	// �f�X�g���N�^
	~SoundBuffer();


	// �r������
	std::mutex mtx;

	// �T�E���h�f�[�^�o�b�t�@
	std::shared_ptr<std::vector<float>>wave;
};
