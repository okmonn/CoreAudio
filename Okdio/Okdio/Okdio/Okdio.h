#pragma once
#include "OkdioEngine.h"

class __declspec(uuid("00000000-0917-0917-0917-000000000001"))
	Okdio :
	public okmonn::Unknown<Okdio>
{
	friend OkdioEngine;

public:
	// �R���X�g���N�^
	Okdio(const std::string& fileName);
	// �f�X�g���N�^
	~Okdio();

	// �Đ�
	void Play(const bool& loop = false);

	// ��~
	void Stop(void);

	// �T�E���h���擾
	okmonn::SoundInfo GetInfo(void) const;

	// �T�E���h���Z�b�g
	void SetInfo(const okmonn::SoundInfo& info);

	// �Ō�܂ōĐ��������m�F
	bool IsFinish(void) const;

private:
	// �ǂݍ���
	void Load(const std::string& fileName);

	// �g�`�f�[�^���T�E���h�L���[�ɒǉ�
	void Submit(const size_t& num);

	Okdio* ptr(void)
	{
		return this;
	}


	// �Q�ƃt�@�C����
	std::string name;

	// �T�E���h���
	okmonn::SoundInfo info;

	// �ǂݍ��݈ʒu
	size_t read;

	// �Đ��t���O
	bool play;

	// ���[�v�t���O
	bool loop;

	// �I���t���O
	bool finish;
};
