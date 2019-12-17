#pragma once
#include "../Infomatin.h"
#include <vector>
#include <complex>

struct _GUID;
typedef _GUID GUID;
class OkdioEngine;

namespace okmonn
{
	// OkdioEngine����
	bool CreateEngine(const okmonn::AudioDevType& devType, const okmonn::AudioType& audioType, const GUID& id, void** engine, const int devIndex = -1);

	// ���j�R�[�h��������}���`�o�C�g�����ɕϊ�
	std::string ChangeCode(const std::wstring& st);

	// �I�[�f�B�I�f�o�C�X���擾
	std::vector<AudioDevProp> GetAudioDevProp(const AudioDevType& type);

	// ���T���v��
	std::vector<float>ReSample(const std::vector<float>& data, const okmonn::SoundInfo& inputInfo, const okmonn::SoundInfo& outputInfo);

	// �K��
	unsigned int Factorial(const unsigned int& val);

	// ����x�b�Z���֐�
	double Vessel(const double& val);

	// �W�{���֐�
	std::vector<double> Sinc(const unsigned char& siderope, const unsigned short& degree, const SampleParam& param);

	// ���z�����̎擾
	unsigned short GetDegree(const unsigned char& siderope, const SampleParam& param);

	// �T���v�����O���g���ϊ��p�����[�^�擾
	SampleParam GetParam(const unsigned int& befor, const unsigned int& after);

	// �n�j���O���֐�
	template <typename T>
	T Hanning(const size_t& index, const size_t& num);

	// �J�C�U�[���֐�
	template <typename T>
	T Kaizer(const unsigned char& siderope);

	// ���T���v�����O
	template <typename T>
	std::vector<T> ReSampling(const std::vector<double>& corre, const SampleParam& param, const std::vector<T>& data, const SoundInfo& info);

	// PSOLA
	template <typename T>
	std::vector<T> PSOLA(const std::vector<T>& data, const okmonn::SoundInfo& info, const T& rate);

	// ���K��
	// �l��-1�`1�ɐ��K������
	template <typename T, typename M>
	T Normalized(const M& val, const T& offset);

	// �K�i��
	template <typename T, typename M>
	T Standarded(const M& val, const size_t& size, const M& offset = 0);

	// ���U�t�[���G�ϊ�
	template <typename T>
	std::vector<std::complex<T>> DFT(const std::vector<T>& data);

	// �t���U�t�[���G�ϊ�
	template <typename T>
	std::vector<T> IDFT(const std::vector<std::complex<T>>& comp);

	// �����t�[���G�ϊ�
	template <typename T>
	std::vector<std::complex<T>> FFT(const std::vector<T>& data);

	// �t�����t�[���G�ϊ�
	template <typename T>
	std::vector<T> IFFT(const std::vector<std::complex<T>>& comp, const size_t& num);
}
