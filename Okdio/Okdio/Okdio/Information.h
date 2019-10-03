#pragma once
#include <string>

namespace okmonn
{
	// �I�[�f�B�I���
	struct AudioInfo
	{
		//�T���v�����O���g��
		unsigned int sample;
		//�ʎq���r�b�g���̃o�C�g��
		unsigned char byte;
		//�`�����l����
		unsigned char channel;
		//float�t���O
		unsigned char flag;

		AudioInfo() {
			sample = byte = channel = flag = 0;
		}
		AudioInfo(const unsigned int& s, const unsigned char& b, const unsigned char& c, const unsigned char& f) :
			sample(s), byte(b), channel(c), flag(f) {}
		AudioInfo(const AudioInfo& info) {
			(*this) = info;
		}
		void operator=(const AudioInfo& info) {
			sample  = info.sample;
			byte    = info.byte;
			channel = info.channel;
			flag    = info.flag;
		}
	};

	// �T���v�����O���g���ϊ��p�����[�^
	struct ConvertParam
	{
		//�ϊ��������T���v�����O���g��
		unsigned int sample;
		//�����A�b�v�T���v�����O�ς݂̃T���v�����O���g��
		unsigned int upSample;
		//�J�b�g�I�t���g��
		unsigned int cutoff;
		//�����A�b�v�T���v�����O�{��
		unsigned char rate;
		//�ϊ���ƕϊ��O�̔{��
		double gap;

		ConvertParam() {
			gap = sample = upSample = cutoff = rate = 0;
		}
	};

	// �I�[�f�B�I�f�o�C�X���
	struct AudioDeviceProp
	{
		//ID
		std::string id;
		//�X�e�[�^�X
		std::string state;
		//�t���l�[��
		std::string fullName;
		//�V���[�g�l�[��
		std::string shortName;
		//�����f�o�C�X��
		std::string deviceName;
	};

	// �I�[�f�B�I�f�o�C�X�^�C�v
	enum AudioDeviceType
	{
		//�X�s�[�J�[
		Render,
		//�}�C�N
		Capture
	};

	// �I�[�f�B�I�Đ��^�C�v
	enum AudioType
	{
		//���L���[�h
		SHARED,
		//�r�����[�h
		EXCLUSIVE
	};
}
