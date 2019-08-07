#pragma once
#include "../etc/Info.h"
#include <vector>

struct _GUID;
typedef _GUID GUID;

namespace okmonn
{
	// ユニコード文字をマルチバイト文字に変換
	std::string ChangeCode(const std::wstring& wstr);

	// オーディオデバイス列挙
	std::vector<okmonn::DeviceProp>GetDeviceProp(const okmonn::DeviceType& type);

	// オーディオエンジン生成
	long CreateAudioEngin(const unsigned int& index, const okmonn::DeviceType& type, const GUID& id, void** obj);
}
