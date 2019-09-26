#pragma once
#include "../ClassBase/Singleton.h"
#include "../Information.h"
#include <vector>
#include <memory>
#include <unordered_map>

class SoundLoader :
	public Singleton<SoundLoader>
{
	struct Sound {
		//�T�E���h���
		okmonn::AudioInfo info;
		//�T�E���h�f�[�^
		std::shared_ptr<std::vector<unsigned char>>wave;
		//�T���v�����O���g���ϊ��p�����[�^
		okmonn::ConvertParam param;
		//�T���v�����O���g���ϊ��p�W��
		std::shared_ptr<std::vector<double>>corre;
	};

	friend Singleton<SoundLoader>;
public:
	// �o�̓T�E���h���Z�b�g
	void SetOutInfo(const okmonn::AudioInfo& info);

	// �ǂݍ���
	int Load(const std::string& fileName);

	// �w��f�[�^���폜
	void Delete(const std::string& fileName);

	// �T�E���h���擾
	okmonn::AudioInfo GetInfo(const std::string& fileName);

	// �T�E���h�f�[�^�擾
	std::shared_ptr<std::vector<unsigned char>> GetWave(const std::string& fileName);

	// �T�E���h�f�[�^���擾
	size_t GetWaveNum(const std::string& fileName);

	// �T���v�����O���g���ϊ��p�����[�^�擾
	okmonn::ConvertParam GetConvertParam(const std::string& fileName);

	// �T���v�����O���g���ϊ��p�W���擾
	std::shared_ptr<std::vector<double>> GetConvertCorre(const std::string& fileName);

private:
	// �R���X�g���N�^
	SoundLoader();
	// �f�X�g���N�^
	~SoundLoader();


	// �o�̓T�E���h���
	okmonn::AudioInfo info;

	// �ǂݍ��݃f�[�^
	std::unordered_map<std::string, Sound>sound;
};
