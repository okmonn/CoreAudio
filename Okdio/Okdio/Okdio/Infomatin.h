#pragma once
#include <string>

namespace okmonn
{
	// オーディオデバイスタイプ
	enum class AudioDevType
	{
		//出力
		output,
		//入力
		input
	};

	// オーディオデバイス情報
	struct AudioDevProp
	{
		//ID
		std::string id;
		//フルネーム
		std::string fullName;
		//ショートネーム
		std::string shortName;
		//物理デバイス名
		std::string devName;
	};

	// オーディオ再生タイプ
	enum class AudioType
	{
		//共有モード
		shared,
		//排他モード
		exclusive
	};

	// サウンド情報
	struct SoundInfo
	{
		//サンプリング周波数
		unsigned int sample;
		//量子化ビット数(バイト表記)
		unsigned char byte;
		//チャンネル数
		unsigned char channel;
		//32bit-float判定フラグ
		unsigned char flag;

		SoundInfo(const unsigned int& s = 0, const unsigned char& b = 0, const unsigned char& c = 0, const unsigned char& f = 0) {
			sample  = s;
			byte    = b;
			channel = c;
			flag    = f;
		}
		SoundInfo(const SoundInfo& info) {
			(*this) = info;
		}
		void operator=(const SoundInfo& info) {
			sample  = info.sample;
			byte    = info.byte;
			channel = info.channel;
			flag    = info.flag;
		}
	};

	// サンプリング周波数変換パラメータ
	struct SampleParam
	{
		//変換したいサンプリング周波数
		unsigned int sample;
		//内部アップサンプリング済みのサンプリング周波数
		unsigned int upSample;
		//カットオフ周波数
		unsigned int cutoff;
		//内部アップサンプリング倍率
		unsigned char rate;
		//変換後と変換前の倍率
		double gap;

		SampleParam() {
			gap = sample = upSample = cutoff = rate = 0;
		}
	};
}
