#pragma once
#include "../Function/Function.h"
#include <string>
#include <memory>

namespace wav
{
	// RIFF���
	struct RIFF
	{
		//ID�i"RIFF"������j
		char id[4];
		//�T�C�Y
		long size;
		//�^�C�v�i"WAVE"������j
		char type[4];
	};

	// FMT���
	struct FMT
	{
		//ID�i"fmt "������j
		char id[4];
		//�T�C�Y
		long size;
		//�^�C�v
		short type;
		//�`�����l����
		short channel;
		//�T���v�����O���g��
		long sample;
		//1�T���v�����O������̃o�C�g��
		long byte;
		//1�u���b�N�T�C�Y
		short block;
		//�ʎq���r�b�g��
		short bit;
	};

	// �ǂݍ���
	long Load(const std::string& fileName, okmonn::AudioInfo& info, std::shared_ptr<std::vector<float>>& data)
	{
		FILE* file = nullptr;
		if (fopen_s(&file, fileName.c_str(), "rb") != 0)
		{
			return -1;
		}

		std::string id, type;

		//RIFF�`�����N�ǂݍ���
		RIFF riff{};
		{
			fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);
			id.assign(&riff.id[0], sizeof(riff.id));
			type.assign(&riff.type[0], sizeof(riff.type));
			if (id != "RIFF" || type != "WAVE")
			{
				fclose(file);
				return -1;
			}
		}

		//FMT�`�����N�ǂݍ���
		FMT fmt{};
		{
			fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);
			id.assign(&fmt.id[0], sizeof(fmt.id));
			if (id != "fmt ")
			{
				fclose(file);
				return -1;
			}
			//FMT�`�����N�̊g�������ǂݍ���
			std::vector<unsigned char>extend(fmt.size - (sizeof(fmt) - sizeof(fmt.id) - sizeof(fmt.size)));
			fread_s(extend.data(), sizeof(unsigned char) * extend.size(), sizeof(unsigned char) * extend.size(), 1, file);
		}

		//DATA�`�����N�܂œǂݔ�΂�
		unsigned long size = 0;
		do
		{
			fseek(file, size, SEEK_CUR);
			fread_s(&id[0], sizeof(unsigned char) * id.size(), sizeof(unsigned char) * id.size(), 1, file);
			fread_s(&size, sizeof(size), sizeof(size), 1, file);
		} while (id != "data");

		//�g�`�f�[�^�ǂݍ���
		info = okmonn::AudioInfo(unsigned int(fmt.sample), unsigned char(fmt.bit / 8), unsigned char(fmt.channel), 0);
		data = std::make_shared<std::vector<float>>(size / info.byte);
		switch (info.byte)
		{
		case 1:
		{
			unsigned char tmp = 0;
			for (float& i : *data)
			{
				fread_s(&tmp, sizeof(tmp), sizeof(tmp), 1, file);
				i = okmonn::Normalize<float>(tmp);
			}
			break;
		}
		case 2:
		{
			short tmp = 0;
			for (float& i : *data)
			{
				fread_s(&tmp, sizeof(tmp), sizeof(tmp), 1, file);
				i = okmonn::Normalize<float>(tmp);
			}
		}
			break;
		default:
			break;
		}

		fread_s(data->data(), size, size, 1, file);

		//�I��
		fclose(file);

		return 0;
	}
}