#pragma once
#include <string>

namespace okmonn
{
	// �I�[�f�B�I�f�o�C�X�^�C�v
	enum class DeviceType
	{
		RENDER,
		CAPTURE,
	};

	// �I�[�f�B�I�f�o�C�X���
	struct DeviceProp
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


	// �T�E���h���
	struct Info
	{
		//�T���v�����O���g��
		unsigned short sample;
		//�ʎq���r�b�g��
		unsigned char bit;
		//�`�����l����
		unsigned char channel;

		//�R���X�g���N�^
		Info() {
			sample = bit = channel = 0;
		}
		Info(const unsigned short& s, const unsigned char& b, const unsigned char& c) {
			sample  = s;
			bit     = b;
			channel = c;
		}
		Info(const Info& info) {
			(*this) = info;
		}

		// ������Z�q�I�[�o�[���[�h
		void operator=(const Info& info) {
			sample  = info.sample;
			bit     = info.bit;
			channel = info.channel;
		}
	};
}
