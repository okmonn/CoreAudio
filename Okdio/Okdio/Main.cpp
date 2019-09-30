#include "Okdio/Okdio.h"
#include <iostream>

std::vector<std::complex<double>> FFT(const std::vector<double>& data)
{
	//虚数単位
	const std::complex<double>Imaginary = std::complex<double>(0, 1);

	//累乗を求める
	unsigned int exponent = int(std::log2(data.size()));

	//計算用バッファ
	std::vector<double>buf(data.size());

	//ビット反転
	std::vector<unsigned int>index(data.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		unsigned int tmp = i;
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}
		buf[i] = data[index[i]];
	}

	std::vector<std::complex<double>>comp(data.size(), 0);
	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::pow(2.0, exponent - stage); ++i)
		{
			for (unsigned int n = 0; n < std::pow(2.0, stage - 1); ++n)
			{
				auto tmp1 = std::exp(-Imaginary * 2.0 * std::acos(-1.0) * double(n) / std::pow(2.0, stage));
			}
		}
	}

	return comp;
}

std::vector<double> IFFT(const std::vector<std::complex<double>>& comp)
{
	//虚数単位
	const std::complex<double>Imaginary = std::complex<double>(0, 1);

	std::vector<double>data(comp.size(), 0);
	std::vector<unsigned int>index(comp.size());

	//累乗を求める
	unsigned int exponent = int(std::log2(comp.size()));

	//配列の入れ替え
	index[0] = 0;
	for (size_t i = 1; i < index.size(); ++i)
	{
		unsigned int tmp = i;
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}
	}

	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int e = 0; e < std::pow(2.0, stage); ++e)
		{
			auto tmp = std::exp(Imaginary * 2.0 * std::acos(-1.0) * double(e) / std::pow(2.0, stage));
			for (unsigned int i = 0; i < std::pow(2.0, exponent - stage); ++i)
			{
				unsigned int No = i * std::pow(2.0, stage) + e;
				if (e / int(std::pow(2.0, stage - 1)) == 0)
				{
					data[No] += (comp[index[No]] + (comp[index[No + std::pow(2.0, stage - 1)]] * tmp)).real();
				}
				else
				{
					data[No] += ((comp[index[No]] * tmp) + comp[index[No - std::pow(2.0, stage - 1)]]).real();
				}
			}
		}
	}

	for (auto& i : data)
	{
		i /= comp.size();
	}

	return data;
}

int main(void)
{
	std::vector<double>A(8);
	for (size_t i = 0; i < A.size(); ++i)
	{
		A[i] = std::sin(2.0 * std::acos(-1.0) * 440.0 * i / 44100);
	}
	auto fft = FFT(A);
	auto ifft = IFFT(fft);

	auto hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	_ASSERT(hr == S_OK);

	{
		Microsoft::WRL::ComPtr<OkdioEngine>engine = nullptr;
		hr = okmonn::CreateOkdioEngine(okmonn::AudioDeviceType::Render, okmonn::AudioType::SHARED, IID_PPV_ARGS(&engine));
		engine->Start();
		Microsoft::WRL::ComPtr<Okdio>okdio = nullptr;
		hr = engine->CreateOkdio("SOS.wav", IID_PPV_ARGS(&okdio));

		while (!(GetKeyState(VK_ESCAPE) & 0x80))
		{

		}

		engine->Stop();
	}
	
	CoUninitialize();

	return 0;
}