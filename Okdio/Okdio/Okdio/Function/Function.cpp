#include "Function.h"
#include "../OkdioEngine.h"
#include <wrl.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <propvarutil.h>
#include <cuda_runtime.h>
#include <functiondiscoverykeys_devpkey.h>

#pragma comment(lib, "Propsys.lib")
#pragma comment(lib, "cudart.lib")

// OkdioEngine����
long okmonn::CreateOkdioEngine(const okmonn::AudioDeviceType& devType, const okmonn::AudioType& audioType, const GUID& id, void** obj)
{
	if (obj == nullptr)
	{
		return E_INVALIDARG;
	}

	*obj = nullptr;
	if (id == __uuidof(OkdioEngine))
	{
		*obj = new OkdioEngine(devType, audioType);
	}
	else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

// ���j�R�[�h��������}���`�o�C�g�����ɕϊ�
std::string okmonn::ChangeCode(const std::wstring& wstr)
{
	std::vector<char> buffer(WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr));
	WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, &buffer.front(), buffer.size(), nullptr, nullptr);
	return std::string(buffer.begin(), buffer.end());
}

// GPU�������m��
long okmonn::CudaMelloc(void** ptr, const size_t& size)
{
	return cudaMalloc(&(*ptr), size);
}

// CPU����������GPU�������ɃR�s�[
long okmonn::CudaCopyToGPU(void** ptr, void* data, const size_t& size)
{
	return cudaMemcpy(*ptr, data, size, cudaMemcpyKind::cudaMemcpyHostToDevice);
}

// GPU����������CPU�������ɃR�s�[
long okmonn::CudaCopyToCPU(void** ptr, void* data, const size_t& size)
{
	return cudaMemcpy(*ptr, data, size, cudaMemcpyKind::cudaMemcpyDeviceToHost);;
}

// GPU���������
long okmonn::CudaFree(void** ptr)
{
	return cudaFree(*ptr);
}

// �I�[�f�B�I�f�o�C�X���擾
std::vector<okmonn::AudioDeviceProp> okmonn::GetAudioDeviceProp(const okmonn::AudioDeviceType& type)
{
	long hr = S_OK;

	//�I�[�f�B�I�f�o�C�X�񋓗p�C���^�[�t�F�[�X
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
		_ASSERT(hr == S_OK);
	}

	//�I�[�f�B�I�f�o�C�X�̃R���N�V�����񋓗p�C���^�[�t�F�[�X
	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	{
		hr = enumerator->EnumAudioEndpoints(EDataFlow(type), DEVICE_STATE_ACTIVE, &collection);
		_ASSERT(hr == S_OK);
	}

	//�R���N�V�������擾
	unsigned int deviceNum = 0;
	{
		hr = collection->GetCount(&deviceNum);
		_ASSERT(hr == S_OK);
	}

	//���ꂼ��̃I�[�f�B�I�f�o�C�X���擾
	std::vector<AudioDeviceProp>deviceProp(deviceNum);
	{
		Microsoft::WRL::ComPtr<IMMDevice>device = nullptr;
		Microsoft::WRL::ComPtr<IPropertyStore>prop = nullptr;
		LPWSTR buf = nullptr;
		PROPVARIANT var{};
		for (unsigned int i = 0; i < deviceNum; ++i)
		{
			//�f�o�C�X�擾
			hr = collection->Item(i, &device);
			_ASSERT(hr == S_OK);

			//ID�擾
			hr = device->GetId(&buf);
			_ASSERT(hr == S_OK);
			deviceProp[i].id = ChangeCode(std::wstring(buf));

			//�X�e�[�^�X�擾
			unsigned long deviceState = 0;
			hr = device->GetState(&deviceState);
			_ASSERT(hr == S_OK);
			switch (deviceState)
			{
			case DEVICE_STATE_ACTIVE:
				deviceProp[i].state = "ACTIVE";
				break;
			case DEVICE_STATE_DISABLED:
				deviceProp[i].state = "DISABLED";
				break;
			case DEVICE_STATE_NOTPRESENT:
				deviceProp[i].state = "NOTPRESENT";
				break;
			case DEVICE_STATE_UNPLUGGED:
				deviceProp[i].state = "UNPLUGGED";
				break;
			default:
				deviceProp[i].state.clear();
				break;
			}

			//�f�o�C�X�̏��̎擾����
			hr = device->OpenPropertyStore(STGM_READ, &prop);
			_ASSERT(hr == S_OK);

			hr = prop->GetValue(PKEY_Device_FriendlyName, &var);
			_ASSERT(hr == S_OK);
			hr = PropVariantToStringAlloc(var, &buf);
			_ASSERT(hr == S_OK);
			deviceProp[i].fullName = ChangeCode(std::wstring(buf));

			hr = prop->GetValue(PKEY_Device_DeviceDesc, &var);
			_ASSERT(hr == S_OK);
			hr = PropVariantToStringAlloc(var, &buf);
			_ASSERT(hr == S_OK);
			deviceProp[i].shortName = ChangeCode(std::wstring(buf));

			hr = prop->GetValue(PKEY_DeviceInterface_FriendlyName, &var);
			_ASSERT(hr == S_OK);
			hr = PropVariantToStringAlloc(var, &buf);
			_ASSERT(hr == S_OK);
			deviceProp[i].shortName = ChangeCode(std::wstring(buf));
		}

		CoTaskMemFree(buf);
	}

	return deviceProp;
}

// �T���v�����O���g���ϊ��p�����̎擾
unsigned short okmonn::GetDegree(const unsigned char& siderope, const okmonn::ConvertParam& param)
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

// �T���v�����O���g���ϊ��p�p�����[�^�̎擾
okmonn::ConvertParam okmonn::GetConvertParam(const unsigned int& befor, const unsigned int& affter)
{
	okmonn::ConvertParam param;
	param.sample   = affter;
	param.rate     = char(std::fmax(befor, affter) / std::fmin(befor, affter)) + 1;
	param.upSample = befor * param.rate;
	param.cutoff   = int((std::fmin(befor, affter) / 2) * 0.9);
	param.gap      = (double(befor) / double(affter)) * param.rate;

	return param;
}

// �K��
unsigned int okmonn::Factorial(const unsigned int& n)
{
	unsigned int tmp = 1;
	for (unsigned int i = 1; i <= n; ++i)
	{
		tmp *= i;
	}

	return tmp;
}

// �J�C�U�[���֐�
double okmonn::Kaizer(const unsigned char& siderope)
{
	if (21 < siderope && siderope < 50)
	{
		return 0.5842 * std::pow(double(siderope - 21), 0.4) + 0.07886 * double(siderope - 21);
	}
	else if (50 <= siderope)
	{
		return 0.1102 * (double(siderope) - 8.7);
	}

	return 0.0;
}

// �n�j���O��
double okmonn::Hanning(const unsigned int& n, const size_t& num)
{
	return 0.5 - 0.5 * std::cos(2.0 * std::acos(-1.0) * n / num);
}

// ��1��x�b�Z���֐�
double okmonn::Vessel(const double& val)
{
	double tmp = 1.0;
	for (unsigned int i = 1; i <= 20; ++i)
	{
		tmp += std::pow(std::pow(val / 2.0, i) / Factorial(i), 2.0);
	}

	return tmp;
}

// �W�{���֐�
std::vector<double> okmonn::Sinc(const unsigned char& siderope, const unsigned short& degree, const okmonn::ConvertParam& param)
{
	std::vector<double>tmp(degree + 1);

	double kaizer = Kaizer(siderope);
	tmp[degree / 2] = (2.0 * double(param.cutoff)) / double(param.upSample);
	for (unsigned short i = 1; i <= degree / 2; ++i)
	{
		double win = Vessel(kaizer * std::sqrt(1.0 - std::pow(double(i) / double(degree / 2), 2.0))) / Vessel(kaizer);
		tmp[degree / 2 + i] = (std::sin(((2.0 * std::acos(-1.0) * double(param.cutoff)) / double(param.upSample)) * i) / (std::acos(-1.0) * i)) * win;
		tmp[degree / 2 - i] = tmp[degree / 2 + i];
	}

	return tmp;
}

// 8bit����32bit-float
template<typename T>
T okmonn::Normalize(const unsigned char& val)
{
	return (T(val) / T(0xff / 2)) - T(1.0);
}
template double okmonn::Normalize(const unsigned char&);
template float okmonn::Normalize(const unsigned char&);

// 16bit����32bit-float
template<typename T>
T okmonn::Normalize(const short& val)
{
	return T(val) / T(0xffff / 2);
}
template double okmonn::Normalize(const short&);
template float okmonn::Normalize(const short&);

// 32bit-float����8bit-unsigned char
unsigned char okmonn::FloatToChar(const float& val)
{
	float tmp = val;
	if (tmp > 1.0f)
	{
		tmp = 1.0f;
	}
	else if (tmp < -1.0f)
	{
		tmp = -1.0f;
	}

	return unsigned char(std::round((tmp + 1.0f) * float(0xff / 2)));
}

// 32bit-float����16bit-short
short okmonn::FloatToShort(const float& val)
{
	float tmp = val;
	if (tmp > 1.0f)
	{
		tmp = 1.0f;
	}
	else if (tmp < -1.0f)
	{
		tmp = -1.0f;
	}

	return short(std::round(tmp * float(0xffff / 2)));
}


// ���U�t�[���G�ϊ�
template<typename T>
std::vector<std::complex<T>> okmonn::DFT(const std::vector<T>& data)
{
	//�����P��
	const std::complex<T>Imaginary(0, 1);

	std::vector<std::complex<T>>comp(data.size(), 0);
	for (size_t i = 0; i < comp.size(); ++i)
	{
		for (size_t n = 0; n < comp.size(); ++n)
		{
			//comp[i] += data[n] * std::exp(-Imaginary * T(2.0) * std::acos(T(-1.0)) * T(i) * T(n) / T(comp.size()));
			comp[i] += std::cos(T(2.0) * std::acos(T(-1.0)) * T(i * n) / T(data.size())) - (Imaginary * std::sin(T(2.0) * std::acos(T(-1.0)) * T(i * n) / T(data.size())));
		}
	}

	return comp;
}
template std::vector<std::complex<double>> okmonn::DFT(const std::vector<double>&);
template std::vector<std::complex<float>> okmonn::DFT(const std::vector<float>&);

// �t���U�t�[���G�ϊ�
template<typename T>
std::vector<T> okmonn::IDFT(const std::vector<std::complex<T>>& comp)
{
	//�����P��
	const std::complex<T>Imaginary = std::complex<T>(0, 1);

	std::vector<T>data(comp.size(), 0);
	for (size_t i = 0; i < data.size(); ++i)
	{
		for (size_t n = 0; n < data.size(); ++n)
		{
			data[i] += (comp[n] * std::exp(Imaginary * T(2.0) * std::acos(T(-1.0)) * T(i) * T(n) / T(data.size()))).real();
		}

		data[i] /= T(data.size());
	}

	return data;
}
template std::vector<double> okmonn::IDFT(const std::vector<std::complex<double>>&);
template std::vector<float> okmonn::IDFT(const std::vector<std::complex<float>>&);

// �����t�[���G�ϊ�
template<typename T>
std::vector<std::complex<T>> okmonn::FFT(const std::vector<T>& data)
{
	//�����P��
	const std::complex<T>Imaginary = std::complex<T>(0, 1);

	//�ݏ�����߂�
	unsigned int exponent = int(std::ceil(std::log2(data.size())));

	//2�̗ݏ�̐�
	unsigned int num = int(std::pow(2.0, exponent));

	//�v�Z�p�o�b�t�@
	std::vector<std::complex<T>>comp(num, 0);

	//�r�b�g���]
	std::vector<unsigned int>index(comp.size());
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
			comp[i] = data[index[i]];
		}
		else
		{
			comp[i] = T(0);
		}
	}

	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::pow(2.0, exponent - stage); ++i)
		{
			for (unsigned int n = 0; n < std::pow(2.0, stage - 1); ++n)
			{
				std::complex<T> corre1 = std::exp(-Imaginary * T(2.0) * std::acos(T(-1.0)) * T(n) / T(std::pow(2.0, stage)));
				std::complex<T> corre2 = std::exp(-Imaginary * T(2.0) * std::acos(T(-1.0)) * T(n + std::pow(2.0, stage - 1)) / T(std::pow(2.0, stage)));

				unsigned int No1 = int(i * std::pow(2.0, stage) + n);
				unsigned int No2 = int(No1 + std::pow(2.0, stage - 1));

				std::complex<T> tmp1 = comp[No1];
				std::complex<T> tmp2 = comp[No2];

				comp[No1] = tmp1 + (tmp2 * corre1);
				comp[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}

	return comp;
}
template std::vector<std::complex<double>> okmonn::FFT(const std::vector<double>&);
template std::vector<std::complex<float>> okmonn::FFT(const std::vector<float>&);

// �t�����t�[���G�ϊ�
template<typename T>
std::vector<T> okmonn::IFFT(const std::vector<std::complex<T>>& comp, const size_t& num)
{
	//�����P��
	const std::complex<T>Imaginary = std::complex<T>(0, 1);

	//�ݏ�����߂�
	unsigned int exponent = int(std::log2(comp.size()));

	//�v�Z�p�o�b�t�@
	std::vector<std::complex<T>>buf(comp.size(), 0);

	//�r�b�g���]
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
				std::complex<T> corre1 = std::exp(Imaginary * T(2.0) * std::acos(T(-1.0)) * T(n) / T(std::pow(2.0, stage)));
				std::complex<T> corre2 = std::exp(Imaginary * T(2.0) * std::acos(T(-1.0)) * T(n + std::pow(2.0, stage - 1)) / T(std::pow(2.0, stage)));

				unsigned int No1 = int(i * std::pow(2.0, stage) + n);
				unsigned int No2 = int(No1 + std::pow(2.0, stage - 1));

				std::complex<T> tmp1 = buf[No1];
				std::complex<T> tmp2 = buf[No2];

				buf[No1] = tmp1 + (tmp2 * corre1);
				buf[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}

	std::vector<T>data(num);
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = buf[i].real() / buf.size();
	}

	return data;
}
template std::vector<double> okmonn::IFFT(const std::vector<std::complex<double>>&, const size_t&);
template std::vector<float> okmonn::IFFT(const std::vector<std::complex<float>>&, const size_t&);
