#pragma once
#include <string>

// デフォルトデバイスインデックス
#define DEFAULT_DEVICE 0xffffffff

namespace okmonn
{
	// オーディオデバイスタイプ
	enum class DeviceType
	{
		RENDER,
		CAPTURE,
	};

	// オーディオタイプ
	enum class AudioType
	{
		SHARED,
		EXCLUSIVE
	};

	// オーディオデバイス情報
	struct DeviceProp
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


	// サウンド情報
	struct Info
	{
		//サンプリング周波数
		unsigned short sample;
		//量子化ビット数
		unsigned char bit;
		//チャンネル数
		unsigned char channel;
		//float型サポート
		bool floating;

		//コンストラクタ
		Info() {
			sample = bit = channel = 0;
			floating = false;
		}
		Info(const unsigned short& s, const unsigned char& b, const unsigned char& c, const bool& f = false) {
			sample   = s;
			bit      = b;
			channel  = c;
			floating = f;
		}
		Info(const Info& info) {
			(*this) = info;
		}

		// 代入演算子オーバーロード
		void operator=(const Info& info) {
			sample   = info.sample;
			bit      = info.bit;
			channel  = info.channel;
			floating = info.floating;
		}
	};
}
