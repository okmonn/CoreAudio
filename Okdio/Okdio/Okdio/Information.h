#pragma once
#include <string>

namespace okmonn
{
	// オーディオ情報
	struct AudioInfo
	{
		//サンプリング周波数
		unsigned int sample;
		//量子化ビット数のバイト数
		unsigned char byte;
		//チャンネル数
		unsigned char channel;
		//floatフラグ
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

	// サンプリング周波数変換パラメータ
	struct ConvertParam
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

		ConvertParam() {
			gap = sample = upSample = cutoff = rate = 0;
		}
	};

	// オーディオデバイス情報
	struct AudioDeviceProp
	{
		//ID
		std::string id;
		//ステータス
		std::string state;
		//フルネーム
		std::string fullName;
		//ショートネーム
		std::string shortName;
		//物理デバイス名
		std::string deviceName;
	};

	// オーディオデバイスタイプ
	enum AudioDeviceType
	{
		//スピーカー
		Render,
		//マイク
		Capture
	};

	// オーディオ再生タイプ
	enum AudioType
	{
		//共有モード
		SHARED,
		//排他モード
		EXCLUSIVE
	};
}
