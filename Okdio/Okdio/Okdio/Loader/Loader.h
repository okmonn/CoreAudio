#pragma once
#include "../Base/Singleton.h"
#include "../Infomatin.h"
#include <vector>
#include <memory>
#include <unordered_map>

class Loader : 
	public okmonn::Singleton<Loader>
{
	friend okmonn::Singleton<Loader>;

	struct Sound {
		//�T�E���h���
		okmonn::SoundInfo info;
		std::shared_ptr<std::vector<float>>wave;
	};

public:
	// �ǂݍ���
	bool Load(const std::string& fileName);

	// �폜
	bool Delete(const std::string& fileName);

	// �T�E���h���擾
	okmonn::SoundInfo GetInfo(const std::string& fileName);

	// �T�E���h�f�[�^�擾
	std::shared_ptr<std::vector<float>> GetWave(const std::string& fileName);

	// �T�E���h�f�[�^���擾
	size_t GetWaveNum(const std::string& fileName);

	// 1��̏����f�[�^��
	size_t ProcessingNum(const std::string& fileName);

private:
	// �R���X�g���N�^
	Loader();
	// �f�X�g���N�^
	~Loader();


	// �ǂݍ��݃f�[�^
	std::unordered_map<std::string, Sound>sound;
};
