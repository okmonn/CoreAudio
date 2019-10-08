#pragma once
#include "OkdioEngine.h"

class __declspec(uuid("00000000-0917-0917-0917-000000000001"))
	Okdio : public UnKnown<Okdio>
{
	friend OkdioEngine;
public:
	// �T�E���h�Q�ƃt�@�C�����擾
	std::string GetName(void) const;

	// �T�E���h���擾
	okmonn::AudioInfo GetInfo(void) const;

	// �T�E���h�f�[�^�擾
	std::weak_ptr<std::vector<float>> GetWave(void) const;

	// �T�E���h�f�[�^���擾
	size_t GetWaveNum(void) const;

	// �T���v�����O���g���ϊ��p�����[�^�擾
	okmonn::ConvertParam GetConvertParam(void) const;

	// �T���v�����O���g���ϊ��p�W���擾
	std::weak_ptr<std::vector<double>> GetConvertCorre(void) const;

private:
	void operator=(const Okdio&) = delete;

	// �R���X�g���N�^
	Okdio(const std::string& fileName);
	// �f�X�g���N�^
	~Okdio();

	// �Q�ƃT�E���h�t�@�C����
	std::string name;

	// �C���f�b�N�X
	unsigned int index;

	// �T���v�����O���g���ϊ��p�I�t�Z�b�g
	unsigned int offset;

	// �T���v�����O���g���ϊ��p����
	double gap;

	// �Đ��t���O
	bool play;

	// ���[�v�t���O
	bool loop;
};

