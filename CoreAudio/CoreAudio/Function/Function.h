#pragma once
#include "../etc/Info.h"
#include <vector>

struct _GUID;
typedef _GUID GUID;

namespace okmonn
{
	// ���j�R�[�h�������}���`�o�C�g�����ɕϊ�
	std::string ChangeCode(const std::wstring& wstr);

	// �I�[�f�B�I�f�o�C�X��
	std::vector<okmonn::DeviceProp>GetDeviceProp(const okmonn::DeviceType& type);

	// �I�[�f�B�I�G���W������
	long CreateAudioEngin(const unsigned int& index, const okmonn::DeviceType& type, const GUID& id, void** obj);
}
