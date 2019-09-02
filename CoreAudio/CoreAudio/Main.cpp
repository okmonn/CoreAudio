#include <cmath>
#include <vector>
#include <iostream>

// カイザー窓
double Kaizer(const unsigned short& siderope)
{
	if (siderope > 21 && siderope < 50)
	{
		return (0.5842 * std::pow(double(siderope - 21), 0.4)) + (0.07886 * double(siderope - 21));
	}
	else if (siderope >= 50)
	{
		return 0.1102 * (double(siderope) - 8.7);
	}

	return 0.0;
}

// 階乗
unsigned int Factorial(const unsigned int& n)
{
	unsigned int tmp = 1;
	for (unsigned int i = 1; i <= n; ++i)
	{
		tmp *= i;
	}

	return tmp;
}

// 第１種変形ベッセル関数
double Vessel(const double& val)
{
	double tmp = 1.0;
	for (unsigned int i = 1; i <= 20; ++i)
	{
		tmp += std::pow(std::pow(val / 2.0, i) / double(Factorial(i)), 2.0);
	}

	return tmp;
}

// 窓関数
double WinFunc(const unsigned int& n, const unsigned short& siderope, const unsigned short& degree)
{
	const double M = double(degree) / 2.0;
	return Vessel(Kaizer(siderope) * std::sqrt(1.0 - std::pow(double(n) / M, 2.0))) / Vessel(Kaizer(siderope));
}

// 標本化関数
double Sinc(const unsigned int& n, const unsigned short& siderope, const unsigned short& degree, const unsigned int& cutoff, const unsigned int& sample)
{
	const double PI = 3.14159265;
	if (n != 0)
	{
		const double tmp = (2.0 * PI * double(cutoff)) / double(sample);
		return (std::sin(tmp * n) / (PI * n)) * WinFunc(n, siderope, degree);
	}

	return (2.0 * double(cutoff)) / double(sample);
}

// 理想フィルタ次数の取得
unsigned short Degree(const unsigned int& nyquist, const unsigned int& passBand, const unsigned int& stopBand, const unsigned short& siderope)
{
	const double PI = 3.14159265;
	double tmp1 = (double(passBand) / double(nyquist)) * PI;
	double tmp2 = (double(stopBand) / double(nyquist)) * PI;
	double tmp = tmp2 - tmp1;

	return unsigned short(double(siderope - 8) / (2.285 * tmp));
}

// サンプリングレート
struct Sampling
{
	unsigned int sample;
	unsigned int cutoff;
};

Sampling Sample(const unsigned int& beffor, const unsigned int& affter)
{
	auto rate = (beffor > affter) ? beffor / affter : affter / beffor;

	Sampling tmp;
	tmp.sample = beffor * (rate + 1);
	tmp.cutoff = (beffor > affter) ? (affter / 2) * 0.9 : (beffor / 2) * 0.9;

	return tmp;
}

// エントリーポイント
int main()
{
	const unsigned short siderope = 50;
	const unsigned short degree = 40;
	const unsigned int sample = 44100;
	const unsigned int cutoff = 20000;
	std::vector<double>coe(degree + 1);
	coe[degree / 2] = Sinc(0, siderope, degree, cutoff, sample);
	for (unsigned int i = 1; i <= degree / 2; ++i)
	{
		coe[degree / 2 + i] = Sinc(i, siderope, degree, cutoff, sample);
		coe[degree / 2 - i] = Sinc(i, siderope, degree, cutoff, sample);
	}

	for (size_t i = 0; i < coe.size(); ++i)
	{
		std::cout << coe[i] << std::endl;
	}

	std::cout << Degree(22050, 3500, 4000, 50) << std::endl;

	Sampling s = Sample(44100, 48000);
	std::cout << s.sample << std::endl;
	std::cout << s.cutoff << std::endl;

	std::vector<double>data(44100);
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = std::sin(2.0 * 3.14159265 * 440.0 * i / data.size());
	}

	getchar();

	return 0;
}