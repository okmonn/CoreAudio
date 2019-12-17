#pragma once
#include "../Infomatin.h"
#include <vector>
#include <complex>

struct _GUID;
typedef _GUID GUID;
class OkdioEngine;

namespace okmonn
{
	// OkdioEngine生成
	bool CreateEngine(const okmonn::AudioDevType& devType, const okmonn::AudioType& audioType, const GUID& id, void** engine, const int devIndex = -1);

	// ユニコード文字からマルチバイト文字に変換
	std::string ChangeCode(const std::wstring& st);

	// オーディオデバイス情報取得
	std::vector<AudioDevProp> GetAudioDevProp(const AudioDevType& type);

	// リサンプル
	std::vector<float>ReSample(const std::vector<float>& data, const okmonn::SoundInfo& inputInfo, const okmonn::SoundInfo& outputInfo);

	// 階乗
	unsigned int Factorial(const unsigned int& val);

	// 第一種ベッセル関数
	double Vessel(const double& val);

	// 標本化関数
	std::vector<double> Sinc(const unsigned char& siderope, const unsigned short& degree, const SampleParam& param);

	// 理想次数の取得
	unsigned short GetDegree(const unsigned char& siderope, const SampleParam& param);

	// サンプリング周波数変換パラメータ取得
	SampleParam GetParam(const unsigned int& befor, const unsigned int& after);

	// ハニング窓関数
	template <typename T>
	T Hanning(const size_t& index, const size_t& num);

	// カイザー窓関数
	template <typename T>
	T Kaizer(const unsigned char& siderope);

	// リサンプリング
	template <typename T>
	std::vector<T> ReSampling(const std::vector<double>& corre, const SampleParam& param, const std::vector<T>& data, const SoundInfo& info);

	// PSOLA
	template <typename T>
	std::vector<T> PSOLA(const std::vector<T>& data, const okmonn::SoundInfo& info, const T& rate);

	// 正規化
	// 値を-1～1に正規化する
	template <typename T, typename M>
	T Normalized(const M& val, const T& offset);

	// 規格化
	template <typename T, typename M>
	T Standarded(const M& val, const size_t& size, const M& offset = 0);

	// 離散フーリエ変換
	template <typename T>
	std::vector<std::complex<T>> DFT(const std::vector<T>& data);

	// 逆離散フーリエ変換
	template <typename T>
	std::vector<T> IDFT(const std::vector<std::complex<T>>& comp);

	// 高速フーリエ変換
	template <typename T>
	std::vector<std::complex<T>> FFT(const std::vector<T>& data);

	// 逆高速フーリエ変換
	template <typename T>
	std::vector<T> IFFT(const std::vector<std::complex<T>>& comp, const size_t& num);
}
