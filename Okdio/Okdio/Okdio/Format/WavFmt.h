#pragma once
#include "../Function/Function.h"
#include <memory>

/*
振幅：std::sqrt(real * real + imag * imag)
位相：std::atan2(imag / real);
パワー：振幅 * 振幅
*/

namespace wav
{
	// RIFFチャンク
	struct RIFF {
		char id[4];
		long size;
		char type[4];
	};
	// fmtチャンク
	struct FMT {
		char id[4];
		long size;
		short type;
		short channel;
		long sample;
		long byte;
		short block;
		short bit;
	};

	// 読み込み
	bool Load(const std::string& fileName, std::shared_ptr<std::vector<float>>& wave, okmonn::SoundInfo& info)
	{
		FILE* file = nullptr;
		fopen_s(&file, fileName.c_str(), "rb");
		if (file != nullptr)
		{
			std::string id;

			//RIFFチャンク読み込み
			RIFF riff{};
			{
				fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);
				id.assign(&riff.id[0], sizeof(riff.id));
				std::string type(&riff.type[0], sizeof(riff.type));
				if (id != "RIFF" || type != "WAVE")
				{
					fclose(file);
					return false;
				}
			}
			//fmtチャンク読み込み
			FMT fmt{};
			{
				fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);
				id.assign(&fmt.id[0], sizeof(riff.id));
				if (id != "fmt ")
				{
					fclose(file);
					return false;
				}

				//fmtチャンクの拡張部分読み込み
				std::vector<unsigned char>extend(fmt.size - (sizeof(fmt) - sizeof(fmt.id) - sizeof(fmt.size)));
				fread_s(extend.data(), extend.size(), extend.size(), 1, file);
			}
			//DATAチャンクまでスキップ
			unsigned long size = 0;
			do
			{
				//現在位置からのスキップ
				fseek(file, size, SEEK_CUR);
				fread_s(&id[0], id.size(), id.size(), 1, file);
				fread_s(&size, sizeof(size), sizeof(size), 1, file);

			} while (id != "data");

			info = okmonn::SoundInfo(fmt.sample, unsigned char(fmt.bit / 8), unsigned char(fmt.channel));
			wave = std::make_shared<std::vector<float>>(size / info.byte);

			//DATAチャンク読み込み
			{
				std::vector<unsigned char>tmp(size);
				fread_s(tmp.data(), tmp.size(), tmp.size(), 1, file);
				fclose(file);

				unsigned int index = 0;
				switch (info.byte)
				{
				case 1:
				{
					for (float& i : *wave)
					{
						i = okmonn::Normalized(tmp[index++], -1.0f);
					}
					break;
				}
				case 2:
				{
					short* ptr = (short*)tmp.data();
					for (float& i : *wave)
					{
						i = okmonn::Normalized(ptr[index++], 0.0f);
					}
					break;
				}
				default:
					break;
				}
			}
			
			auto psola = okmonn::PSOLA(*wave, info, 0.5f);
			info.sample /= 0.5f;
			auto param = okmonn::GetParam(info.sample, 48000);
			auto degree = okmonn::GetDegree(100, param);
			auto corre = okmonn::Sinc(100, degree, param);
			auto re = okmonn::ReSampling(corre, param, psola, info);
			std::swap(*wave, re);

			return true;
		}

		return false;
	}
}
