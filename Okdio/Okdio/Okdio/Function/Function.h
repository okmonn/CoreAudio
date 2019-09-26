#pragma once
#include "../Information.h"
#include <vector>
#include <complex>

struct _GUID;
typedef _GUID GUID;

namespace okmonn
{
	// OkdioEngine����
	long CreateOkdioEngine(const okmonn::AudioDeviceType& devType, const okmonn::AudioType& audioType, const GUID& id, void** obj);

	// ���j�R�[�h��������}���`�o�C�g�����ɕϊ�
	std::string ChangeCode(const std::wstring& wstr);

	// GPU�������m��
	long CudaMelloc(void** ptr, const size_t& size);

	// CPU����������GPU�������ɃR�s�[
	long CudaCopyToGPU(void** ptr, void* data, const size_t& size);
	
	// GPU����������CPU�������ɃR�s�[
	long CudaCopyToCPU(void** ptr, void* data, const size_t& size);

	// GPU���������
	long CudaFree(void** ptr);

	// �I�[�f�B�I�f�o�C�X���擾
	std::vector<AudioDeviceProp> GetAudioDeviceProp(const AudioDeviceType& type);

	// �T���v�����O���g���ϊ��p�����̎擾
	unsigned short GetDegree(const unsigned char& siderope, const ConvertParam& param);

	// �T���v�����O���g���ϊ��p�p�����[�^�̎擾
	ConvertParam GetConvertParam(const unsigned int& befor, const unsigned int& affter);

	// �K��
	unsigned int Factorial(const unsigned int& n);

	// �J�C�U�[���֐�
	double Kaizer(const unsigned char& siderope);

	// ��1��x�b�Z���֐�
	double Vessel(const double& val);

	// �W�{���֐�
	std::vector<double> Sinc(const unsigned char& siderope, const unsigned short& degree, const ConvertParam& param);

	template <typename T>
	T Normalize(const unsigned char& val);

	template <typename T>
	T Normalize(const short& val);

	// ���U�t�[���G�ϊ�
	template <typename T>
	std::vector<std::complex<T>> DFT(const std::vector<T>& data);

	// �t���U�t�[���G�ϊ�
	template <typename T>
	std::vector<T> IDFT(const std::vector<std::complex<T>>& comp);
}
