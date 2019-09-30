#include "Okdio/Okdio.h"
#include <iostream>

std::vector<std::complex<double>> FFT(const std::vector<double>& data)
{
	//虚数単位
	const std::complex<double>Imaginary = std::complex<double>(0, 1);

	//累乗を求める
	unsigned int exponent = int(std::log2(data.size()));

	//計算用バッファ
	std::vector<std::complex<double>>comp(data.size(), 0);

	//ビット反転
	std::vector<unsigned int>index(comp.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		unsigned int tmp = i;
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}
		comp[i] = data[index[i]];
	}

	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::pow(2.0, exponent - stage); ++i)
		{
			for (unsigned int n = 0; n < std::pow(2.0, stage - 1); ++n)
			{
				auto corre1 = std::exp(-Imaginary * 2.0 * std::acos(-1.0) * double(n) / std::pow(2.0, stage));
				auto corre2 = std::exp(-Imaginary * 2.0 * std::acos(-1.0) * double(n + std::pow(2.0, stage - 1)) / std::pow(2.0, stage));

				auto No1 = i * std::pow(2.0, stage) + n;
				auto No2 = No1 + std::pow(2.0, stage - 1);

				auto tmp1 = comp[No1];
				auto tmp2 = comp[No2];

				comp[No1] = tmp1 + (tmp2 * corre1);
				comp[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}

	return comp;
}

std::vector<double> IFFT(const std::vector<std::complex<double>>& comp)
{
	//虚数単位
	const std::complex<double>Imaginary = std::complex<double>(0, 1);

	//累乗を求める
	unsigned int exponent = int(std::log2(comp.size()));

	//計算用バッファ
	std::vector<std::complex<double>>buf(comp.size(), 0);

	//ビット反転
	std::vector<unsigned int>index(comp.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		unsigned int tmp = i;
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}
		buf[i] = comp[index[i]];
	}

	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::pow(2.0, exponent - stage); ++i)
		{
			for (unsigned int n = 0; n < std::pow(2.0, stage - 1); ++n)
			{
				auto corre1 = std::exp(Imaginary * 2.0 * std::acos(-1.0) * double(n) / std::pow(2.0, stage));
				auto corre2 = std::exp(Imaginary * 2.0 * std::acos(-1.0) * double(n + std::pow(2.0, stage - 1)) / std::pow(2.0, stage));

				auto No1 = i * std::pow(2.0, stage) + n;
				auto No2 = No1 + std::pow(2.0, stage - 1);

				auto tmp1 = buf[No1];
				auto tmp2 = buf[No2];

				buf[No1] = tmp1 + (tmp2 * corre1);
				buf[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}

	std::vector<double>data(comp.size());
	for (size_t i = 0; i < buf.size(); ++i)
	{
		data[i] = buf[i].real() / buf.size();
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