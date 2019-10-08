#pragma once
#include "../Function/Function.h"
#include <string>
#include <memory>

namespace wav
{
	// RIFF情報
	struct RIFF
	{
		//ID（"RIFF"が入る）
		char id[4];
		//サイズ
		long size;
		//タイプ（"WAVE"が入る）
		char type[4];
	};

	// FMT情報
	struct FMT
	{
		//ID（"fmt "が入る）
		char id[4];
		//サイズ
		long size;
		//タイプ
		short type;
		//チャンネル数
		short channel;
		//サンプリング周波数
		long sample;
		//1サンプリング当たりのバイト数
		long byte;
		//1ブロックサイズ
		short block;
		//量子化ビット数
		short bit;
	};

	// 読み込み
	long Load(const std::string& fileName, okmonn::AudioInfo& info, std::shared_ptr<std::vector<float>>& data)
	{
		FILE* file = nullptr;
		if (fopen_s(&file, fileName.c_str(), "rb") != 0)
		{
			return -1;
		}

		std::string id, type;

		//RIFFチャンク読み込み
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

		//FMTチャンク読み込み
		FMT fmt{};
		{
			fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);
			id.assign(&fmt.id[0], sizeof(fmt.id));
			if (id != "fmt ")
			{
				fclose(file);
				return -1;
			}
			//FMTチャンクの拡張部分読み込み
			std::vector<unsigned char>extend(fmt.size - (sizeof(fmt) - sizeof(fmt.id) - sizeof(fmt.size)));
			fread_s(extend.data(), sizeof(unsigned char) * extend.size(), sizeof(unsigned char) * extend.size(), 1, file);
		}

		//DATAチャンクまで読み飛ばし
		unsigned long size = 0;
		do
		{
			fseek(file, size, SEEK_CUR);
			fread_s(&id[0], sizeof(unsigned char) * id.size(), sizeof(unsigned char) * id.size(), 1, file);
			fread_s(&size, sizeof(size), sizeof(size), 1, file);
		} while (id != "data");

		//波形データ読み込み
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

		//終了
		fclose(file);

		return 0;
	}
}