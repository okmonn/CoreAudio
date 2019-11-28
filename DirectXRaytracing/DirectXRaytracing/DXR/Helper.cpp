#include "Helper.h"
#include <vector>
#include <Windows.h>

// �}���`�o�C�g���������j�R�[�h�����ɕϊ�
std::wstring DXR::ChangeCode(const std::string& str)
{
	std::vector<wchar_t>buf(MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &buf.front(), int(buf.size()));

	return std::wstring(buf.begin(), buf.end());
}