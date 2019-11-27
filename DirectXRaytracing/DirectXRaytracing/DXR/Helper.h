#pragma once
#include <string>

namespace DXR
{
	// マルチバイト文字をユニコード文字に変換
	std::wstring ChangeCode(const std::string& str);
}
