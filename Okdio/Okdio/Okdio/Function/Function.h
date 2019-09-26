#pragma once
#include "../Information.h"
#include <vector>
#include <complex>

struct _GUID;
typedef _GUID GUID;

namespace okmonn
{
	// OkdioEngine生成
	long CreateOkdioEngine(const okmonn::AudioDeviceType& devType, const okmonn::AudioType& audioType, const GUID& id, void** obj);

	// ユニコード文字からマルチバイト文字に変換
	std::string ChangeCode(const std::wstring& wstr);

	// GPUメモリ確保
	long CudaMelloc(void** ptr, const size_t& size);

	// CPUメモリからGPUメモリにコピー
	long CudaCopyToGPU(void** ptr, void* data, const size_t& size);
	
	// GPUメモリからCPUメモリにコピー
	long CudaCopyToCPU(void** ptr, void* data, const size_t& size);

	// GPUメモリ解放
	long CudaFree(void** ptr);

	// オーディオデバイス情報取得
	std::vector<AudioDeviceProp> GetAudioDeviceProp(const AudioDeviceType& type);

	// サンプリング周波数変換用次数の取得
	unsigned short GetDegree(const unsigned char& siderope, const ConvertParam& param);

	// サンプリング周波数変換用パラメータの取得
	ConvertParam GetConvertParam(const unsigned int& befor, const unsigned int& affter);

	// 階乗
	unsigned int Factorial(const unsigned int& n);

	// カイザー窓関数
	double Kaizer(const unsigned char& siderope);

	// 第1種ベッセル関数
	double Vessel(const double& val);

	// 標本化関数
	std::vector<double> Sinc(const unsigned char& siderope, const unsigned short& degree, const ConvertParam& param);

	template <typename T>
	T Normalize(const unsigned char& val);

	template <typename T>
	T Normalize(const short& val);

	// 離散フーリエ変換
	template <typename T>
	std::vector<std::complex<T>> DFT(const std::vector<T>& data);

	// 逆離散フーリエ変換
	template <typename T>
	std::vector<T> IDFT(const std::vector<std::complex<T>>& comp);
}
