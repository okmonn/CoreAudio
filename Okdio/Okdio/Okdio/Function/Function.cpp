#include "../OkdioEngine.h"
#include <cmath>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys_devpkey.h>

// OkdioEngine生成
bool okmonn::CreateEngine(const okmonn::AudioDevType& devType, const okmonn::AudioType& audioType, 
	const GUID& id, void** engine, const int devIndex)
{
	if (id == __uuidof(OkdioEngine))
	{
		*engine = new OkdioEngine(devType, audioType, devIndex);
		return true;
	}

	return false;
}

// ユニコード文字からマルチバイト文字に変換
std::string okmonn::ChangeCode(const std::wstring& st)
{
	std::vector<char>buf(WideCharToMultiByte(CP_OEMCP, 0, st.c_str(), -1, nullptr, 0, nullptr, nullptr));
	WideCharToMultiByte(CP_OEMCP, 0, st.c_str(), -1, buf.data(), buf.size(), nullptr, nullptr);

	return std::string(buf.begin(), buf.end());
}

// オーディオデバイス情報取得
std::vector<okmonn::AudioDevProp> okmonn::GetAudioDevProp(const okmonn::AudioDevType& type)
{
	//オーディオデバイス列挙
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	{
		auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
		_ASSERT(hr == S_OK);
	}
	//オーディオデバイスコレクション
	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	{
		auto hr = enumerator->EnumAudioEndpoints(EDataFlow(type), DEVICE_STATE_ACTIVE, &collection);
		_ASSERT(hr == S_OK);
	}
	//列挙数取得
	unsigned int devNum = 0;
	{
		auto hr = collection->GetCount(&devNum);
		_ASSERT(hr == S_OK);
	}
	//それぞれのデバイス情報取得
	std::vector<AudioDevProp>prop(devNum);
	{
		Microsoft::WRL::ComPtr<IMMDevice>dev = nullptr;
		Microsoft::WRL::ComPtr<IPropertyStore>store = nullptr;
		LPWSTR buf = nullptr;
		PROPVARIANT variant{};
		for (unsigned int i = 0; i < devNum; ++i)
		{
			//デバイス取得
			auto hr = collection->Item(i, &dev);
			_ASSERT(hr == S_OK);

			//ID取得
			hr = dev->GetId(&buf);
			_ASSERT(hr == S_OK);
			prop[i].id = ChangeCode(std::wstring(buf));

			//デバイス名取得準備
			hr = dev->OpenPropertyStore(STGM_READ, &store);
			_ASSERT(hr == S_OK);

			//フルネーム取得
			hr = store->GetValue(PKEY_Device_FriendlyName, &variant);
			_ASSERT(hr == S_OK);
			hr = PropVariantToStringAlloc(variant, &buf);
			_ASSERT(hr == S_OK);
			prop[i].fullName = ChangeCode(std::wstring(buf));

			//ショートネーム取得
			hr = store->GetValue(PKEY_Device_DeviceDesc, &variant);
			_ASSERT(hr == S_OK);
			hr = PropVariantToStringAlloc(variant, &buf);
			_ASSERT(hr == S_OK);
			prop[i].shortName = ChangeCode(std::wstring(buf));

			//物理デバイス名取得
			hr = store->GetValue(PKEY_DeviceInterface_FriendlyName, &variant);
			_ASSERT(hr == S_OK);
			hr = PropVariantToStringAlloc(variant, &buf);
			_ASSERT(hr == S_OK);
			prop[i].devName = ChangeCode(std::wstring(buf));
		}

		//バッファ解放
		CoTaskMemFree(buf);
	}

	return prop;
}

// 階乗
unsigned int okmonn::Factorial(const unsigned int& val)
{
	unsigned int tmp = 1;
	for (unsigned int i = 1; i <= val; ++i)
	{
		tmp *= i;
	}

	return tmp;
}

// 第一種ベッセル関数
double okmonn::Vessel(const double& val)
{
	double tmp = 1;
	for (unsigned int i = 1; i <= 20; ++i)
	{
		tmp += std::pow(std::pow(val / 2.0, i) / Factorial(i), 2.0);
	}

	return tmp;
}

// 標本化関数
std::vector<double> okmonn::Sinc(const unsigned char& siderope, const unsigned short& degree, const SampleParam& param)
{
	std::vector<double>tmp(degree + 1);

	double kaizer = Kaizer<double>(siderope);
	tmp[degree / 2] = (2.0 * double(param.cutoff)) / double(param.upSample);
	for (unsigned short i = 1; i <= degree / 2; ++i)
	{
		double win = Vessel(kaizer * std::sqrt(1.0 - std::pow(double(i) / double(degree / 2), 2.0))) / Vessel(kaizer);
		tmp[degree / 2 + i] = (std::sin(((2.0 * std::acos(-1.0) * double(param.cutoff)) / double(param.upSample)) * i) / (std::acos(-1.0) * i)) * win;
		tmp[degree / 2 - i] = tmp[degree / 2 + i];
	}

	return tmp;
}

// 理想次数の取得
unsigned short okmonn::GetDegree(const unsigned char& siderope, const SampleParam& param)
{
	double tmp1 = (double((param.sample / 2) * 0.9) / double(param.upSample / 2)) * std::acos(-1.0);
	double tmp2 = (double(param.sample / 2) / double(param.upSample / 2)) * std::acos(-1.0);
	double tmp  = tmp2 - tmp1;
	short r = short(std::ceil(double(siderope - 8) / (2.285 * tmp)));
	if (r % 2 != 0)
	{
		++r;
	}

	return r;
}

// サンプリング周波数変換パラメータ取得
okmonn::SampleParam okmonn::GetParam(const unsigned int& befor, const unsigned int& after)
{
	okmonn::SampleParam param;
	param.sample   = after;
	param.rate     = char(std::fmax(befor, after) / std::fmin(befor, after)) + 1;
	param.upSample = befor * param.rate;
	param.cutoff   = int((std::fmin(befor, after) / 2) * 0.9);
	param.gap      = (double(befor) / double(after)) * param.rate;

	return param;
}

// ハニング窓関数
template<typename T>
T okmonn::Hanning(const size_t& index, const size_t& num)
{
	return T(0.5 - 0.5 * std::cos(2.0 * std::acos(-1.0) * index / num));
}
template double okmonn::Hanning(const size_t&, const size_t&);
template float okmonn::Hanning(const size_t&, const size_t&);

// カイザー窓関数
template<typename T>
T okmonn::Kaizer(const unsigned char& siderope)
{
	if (21 < siderope && siderope < 50)
	{
		return T(0.5842 * std::pow(double(siderope - 21), 0.4) + 0.07886 * double(siderope - 21));
	}
	else if (50 <= siderope)
	{
		return T(0.1102 * (double(siderope) - 8.7));
	}

	return 0;
}
template double okmonn::Kaizer(const unsigned char&);
template float okmonn::Kaizer(const unsigned char&);

// リサンプリング
template<typename T>
std::vector<T> okmonn::ReSampling(const std::vector<double>& corre, const SampleParam& param, const std::vector<T>& data, const SoundInfo& info)
{
	std::vector<T>convert(data.size() * (double(param.sample) / double(info.sample)), 0);
	unsigned int offset = (corre.size() - 1) / 2;
	unsigned int index  = 0;
	double gap          = param.gap;
	while (index < convert.size())
	{
		double integer = 0.0;
		gap = std::modf(gap, &integer);
		offset += int(integer);
		//変換
		for (size_t i = 0; i < (corre.size() - 1); ++i)
		{
			long tmp = offset - ((corre.size() - 1) / 2) + i;
			if (size_t(tmp / param.rate) * info.channel >= data.size())
			{
				break;
			}

			if (tmp % param.rate == 0)
			{
				double comp = ((corre[i + 1] - corre[i]) * (1.0 - gap)) + corre[i];
				if (i == ((corre.size() - 1) / 2) - 1)
				{
					comp = ((corre[((corre.size() - 1) / 2) - 1] - corre[((corre.size() - 1) / 2)]) * std::pow(-gap, 2.0)) + corre[((corre.size() - 1) / 2)];
				}
				else if (i == (corre.size() - 1) / 2)
				{
					comp = ((corre[((corre.size() - 1) / 2) - 1] - corre[((corre.size() - 1) / 2)]) * std::pow((1.0 - gap), 2.0)) + corre[((corre.size() - 1) / 2)];
				}

				for (unsigned char ch = 0; ch < info.channel; ++ch)
				{
					if ((tmp / param.rate) * info.channel + ch < data.size())
					{
						convert[index + ch] += data[(tmp / param.rate) * info.channel + ch] * T(comp);
					}
				}
			}
		}

		//ゲイン調節
		for (unsigned char ch = 0; ch < info.channel; ++ch)
		{
			if (index + ch < convert.size())
			{
				convert[index + ch] *= T(param.rate - 0.2);
			}
		}

		index += info.channel;
		gap   += param.gap;
	}

	return convert;
}
template std::vector<double> okmonn::ReSampling(const std::vector<double>&, const okmonn::SampleParam&, const std::vector<double>&, const okmonn::SoundInfo&);
template std::vector<float> okmonn::ReSampling(const std::vector<double>&, const okmonn::SampleParam&, const std::vector<float>&, const okmonn::SoundInfo&);

// PSOLA
template<typename T>
std::vector<T> okmonn::PSOLA(const std::vector<T>& data, const okmonn::SoundInfo& info, const T& rate)
{
	//相関関数範囲
	const unsigned int size = (info.sample * info.channel) / 100;
	//ずらし間隔最小値
	const unsigned int min = size / 2;
	//ずらし間隔最大値
	const unsigned int max = size * 2;

	std::vector<T>convert(data.size() * rate);

	unsigned int offset0 = 0;
	unsigned int offset1 = 0;

	while (offset0 + max < data.size())
	{
		T peak = T(0);
		unsigned int index = min;
		for (unsigned int i = min; i <= max; ++i)
		{
			T tmp = T(0);
			for (unsigned int n = 0; n < size; ++n)
			{
				tmp += data[n] * data[i + n];
			}

			//ピーク値更新
			if (peak < tmp)
			{
				peak  = tmp;
				index = i;
			}
		}

		unsigned int tmp = int(index * rate);
		std::vector<T>part(&data[size_t(std::fmin(offset0 + index - tmp, 0))], &data[offset0 + index + tmp]);
		for (size_t i = 0; i < part.size(); ++i)
		{
			if (offset1 + i < convert.size())
			{
				convert[offset1 + i] += part[i] * Hanning<T>(i, part.size());
			}
		}

		offset0 += index;
		offset1 += part.size() / 2;
	}

	return convert;
}
template std::vector<double> okmonn::PSOLA(const std::vector<double>&, const okmonn::SoundInfo&, const double&);
template std::vector<float> okmonn::PSOLA(const std::vector<float>&, const okmonn::SoundInfo&, const float&);

// 正規化
// 値を-1～1に正規化する
template<typename T, typename M>
T okmonn::Normalized(const M& val, const T& offset)
{
	return (T(val) / T((std::pow(2, sizeof(M) * 8)) / 2)) + offset;
}
template float okmonn::Normalized(const unsigned char&, const float&);
template float okmonn::Normalized(const short&, const float&);

// 規格化
template<typename T, typename M>
T okmonn::Standarded(const M& val, const size_t& size, const M& offset)
{
	M tmp = val;
	if (tmp > M(1))
	{
		tmp = M(1);
	}
	else if (tmp < M(-1))
	{
		tmp = M(-1);
	}

	return T(std::round((tmp + offset) * ((std::pow(2, size * 8) - 1) / 2)));
}
template int okmonn::Standarded(const float&, const size_t&, const float&);
template short okmonn::Standarded(const float&, const size_t&, const float&);
template unsigned char okmonn::Standarded(const float&, const size_t&, const float&);

// 離散フーリエ変換
template<typename T>
std::vector<std::complex<T>> okmonn::DFT(const std::vector<T>& data)
{
	//虚数単位
	const std::complex<T>imaginary(0, 1);

	std::vector<std::complex<T>>convert(data.size(), 0);
	for (size_t i = 0; i < convert.size(); ++i)
	{
		for (size_t n = 0; n < convert.size(); ++n)
		{
			convert[i] += data[n] * std::exp(-imaginary * T(2) * std::acos(T(-1)) * T(i * n) / T(convert.size()));
		}
	}

	return convert;
}
template std::vector<std::complex<double>> okmonn::DFT(const std::vector<double>&);
template std::vector<std::complex<float>> okmonn::DFT(const std::vector<float>&);

// 逆離散フーリエ変換
template<typename T>
std::vector<T> okmonn::IDFT(const std::vector<std::complex<T>>& comp)
{
	//虚数単位
	const std::complex<T>imaginary(0, 1);

	std::vector<T>convert(comp.size(), 0);
	for (size_t i = 0; i < convert.size(); ++i)
	{
		for (size_t n = 0; n < convert.size(); ++n)
		{
			convert[i] += (comp[n] * std::exp(-imaginary * T(2) * std::acos(T(-1)) * T(i * n) / T(convert.size()))).real();
		}

		convert[i] /= T(comp.size());
	}

	return convert;
}
template std::vector<double> okmonn::IDFT(const std::vector<std::complex<double>>&);
template std::vector<float> okmonn::IDFT(const std::vector<std::complex<float>>&);

// 高速フーリエ変換
template<typename T>
std::vector<std::complex<T>> okmonn::FFT(const std::vector<T>& data)
{
	//虚数単位
	const std::complex<T>imaginary(0, 1);

	//指数を求める
	unsigned int exponent = int(std::ceil(std::log2(data.size())));

	//全体数を求める
	unsigned int num = int(std::pow(2, exponent));

	//計算用バッファ
	std::vector<std::complex<T>>convert(num, 0);

	//ビット反転
	std::vector<size_t>index(convert.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		unsigned int tmp = i;
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}

		if (index[i] < data.size())
		{
			convert[i] = data[index[i]];
		}
		else
		{
			convert[i] = T(0);
		}
	}

	//バタフライ演算
	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::pow(2, exponent - stage); ++i)
		{
			for (unsigned int n = 0; n < std::pow(2, stage - 1); ++n)
			{
				std::complex<T>corre1 = std::exp(-imaginary * T(2) * std::acos(T(-1)) * T(n) / T(std::pow(2, stage)));
				std::complex<T>corre2 = std::exp(-imaginary * T(2) * std::acos(T(-1)) * T(n + std::pow(2, stage - 1)) / T(std::pow(2, stage)));

				unsigned int No1 = int(i * std::pow(2, stage) + n);
				unsigned int No2 = int(No1 + std::pow(2, stage - 1));

				std::complex<T>tmp1 = convert[No1];
				std::complex<T>tmp2 = convert[No2];

				convert[No1] = tmp1 + (tmp2 * corre1);
				convert[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}

	return convert;
}
template std::vector<std::complex<double>> okmonn::FFT(const std::vector<double>&);
template std::vector<std::complex<float>> okmonn::FFT(const std::vector<float>&);

// 逆高速フーリエ変換
template<typename T>
std::vector<T> okmonn::IFFT(const std::vector<std::complex<T>>& comp, const size_t& num)
{
	//虚数単位
	const std::complex<T>imaginary(0, 1);

	//指数を求める
	unsigned int exponent = int(std::log2(comp.size()));

	//計算用バッファ
	std::vector<std::complex<T>>convert(comp.size(), 0);

	//ビット反転
	std::vector<size_t>index(convert.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		unsigned int tmp = i;
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}

		convert[i] = comp[index[i]];
	}

	//バタフライ演算
	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::pow(2, exponent - stage); ++i)
		{
			for (unsigned int n = 0; n < std::pow(2, stage - 1); ++n)
			{
				std::complex<T>corre1 = std::exp(imaginary * T(2) * std::acos(T(-1)) * T(n) / T(std::pow(2, stage)));
				std::complex<T>corre2 = std::exp(imaginary * T(2) * std::acos(T(-1)) * T(n * std::pow(2, stage - 1)) / T(std::pow(2, stage)));

				unsigned int No1 = int(i * std::pow(2, stage) + n);
				unsigned int No2 = int(No1 + std::pow(2, stage - 1));

				std::complex<T>tmp1 = convert[No1];
				std::complex<T>tmp2 = convert[No2];

				convert[No1] = tmp1 + (tmp2 * corre1);
				convert[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}

	std::vector<T>data(num);
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = convert[i].real() / convert.size();
	}

	return data;
}
template std::vector<double> okmonn::IFFT(const std::vector<std::complex<double>>&, const size_t&);
template std::vector<float> okmonn::IFFT(const std::vector<std::complex<float>>&, const size_t&);
