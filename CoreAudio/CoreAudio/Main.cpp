#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>

#include <initguid.h>
#include <mmdeviceapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>

#pragma comment(lib, "Propsys.lib")

namespace
{
	using prop_string_t = std::unique_ptr<WCHAR, decltype(&CoTaskMemFree)>;
	#define prop_string_null prop_string_t(nullptr, CoTaskMemFree)

	// �f�o�C�XID�擾
	prop_string_t GetDeviceID(IMMDevice* device)
	{
		LPWSTR buf = nullptr;
		if (device->GetId(&buf) == S_OK)
		{
			return prop_string_t(buf, CoTaskMemFree);
		}
		return prop_string_null;
	}

	// �f�o�C�X�X�e�[�^�X�擾
	std::wstring GetDeviceState(IMMDevice* device)
	{
		unsigned long state = 0;
		if (device->GetState(&state) == S_OK)
		{
			switch (state)
			{
			case DEVICE_STATE_ACTIVE:
				return L"ACTIVE";
				break;
			case DEVICE_STATE_DISABLED:
				return L"DISABLED";
				break;
			case DEVICE_STATE_NOTPRESENT:
				return L"NOTPRESENT";
				break;
			case DEVICE_STATE_UNPLUGGED:
				return L"UNPLUGGED";
				break;
			default:
				break;
			}
		}
		return L"UNKNOWN";
	}

	// �f�o�C�X�v���p�e�B�擾
	prop_string_t GetDeviceProp(IPropertyStore* prop, const PROPERTYKEY& key)
	{
		PROPVARIANT var{};
		PropVariantInit(&var);
		if (prop->GetValue(key, &var) == S_OK)
		{
			LPWSTR str = nullptr;
			if (PropVariantToStringAlloc(var, &str) == S_OK)
			{
				PropVariantClear(&var);
				return prop_string_t(str, CoTaskMemFree);
			}
		}
		return prop_string_null;
	}
}

std::string ChangeCode(const std::wstring& wstr)
{
	std::vector<char> buffer(WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr));
	WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, &buffer.front(), buffer.size(), nullptr, nullptr);
	return std::string(buffer.begin(), buffer.end());
}

int main(void)
{
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//�G���h�|�C���g�f�o�C�X�̗�
	{
		IMMDeviceEnumerator* enumerator = nullptr;
		IMMDeviceCollection* collection = nullptr;
		IMMDevice* device = nullptr;
		IPropertyStore* prop = nullptr;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
		hr = enumerator->EnumAudioEndpoints(EDataFlow::eRender, DEVICE_STATE_ACTIVE, &collection);
		
		unsigned int deviceMax = 0;
		hr = collection->GetCount(&deviceMax);
		for (unsigned int i = 0; i < deviceMax; ++i)
		{
			hr = collection->Item(i, &device);
			hr = device->OpenPropertyStore(STGM_READ, &prop);

			std::cout << i                 << std::endl;
			std::cout << "ID="             << ChangeCode(GetDeviceID(device).get()).c_str()<< std::endl;
			std::cout << "STATE="          << ChangeCode(GetDeviceState(device)).c_str() << std::endl;
			std::cout << "�t���l�[��="     << ChangeCode(GetDeviceProp(prop, PKEY_Device_FriendlyName).get()).c_str() << std::endl;
			std::cout << "�V���[�g�l�[��=" << ChangeCode(GetDeviceProp(prop, PKEY_Device_DeviceDesc).get()).c_str() << std::endl;
			std::cout << "�����f�o�C�X��=" << ChangeCode(GetDeviceProp(prop, PKEY_DeviceInterface_FriendlyName).get()).c_str() << std::endl;
		}

		prop->Release();
		device->Release();
		collection->Release();
		enumerator->Release();
	}

	// �f�t�H���g�G���h�|�C���g�f�o�C�X�Z�b�g
	IMMDevice* device = nullptr;
	{
		IMMDeviceEnumerator* enumerator = nullptr;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
		enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eConsole, &device);
	}

	// �I�[�f�B�I�N���C�A���g�̏���
	IAudioClient* audio = nullptr;
	unsigned int block = 0;
	{
		hr = device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, (void**)& audio);

		//�t�H�[�}�b�g�̃`�F�b�N
		IPropertyStore* prop = nullptr;
		hr = device->OpenPropertyStore(STGM_READ, &prop);
		PROPVARIANT var{};
		PropVariantInit(&var);
		prop->GetValue(PKEY_AudioEngine_DeviceFormat, &var);
		WAVEFORMATEXTENSIBLE* fmt = (WAVEFORMATEXTENSIBLE*)var.blob.pBlobData;
		hr = audio->IsFormatSupported(_AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)fmt, nullptr);
		
		//���C�e���V�擾
		REFERENCE_TIME defPeriod = 0;
		REFERENCE_TIME minPeriod = 0;
		hr = audio->GetDevicePeriod(&defPeriod, &minPeriod);

		//������
		hr = audio->Initialize(_AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
			defPeriod, defPeriod, (WAVEFORMATEX*)fmt, nullptr);
		if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
		{
			//�C����̃t���[�������擾
			unsigned __int32 fream = 0;
			hr = audio->GetBufferSize(&fream);
			defPeriod = REFERENCE_TIME(std::round(10000.0 * 1000 * fream / fmt->Format.nSamplesPerSec));

			//�Ē���
			hr = audio->Initialize(_AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_EXCLUSIVE, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
				defPeriod, defPeriod, (WAVEFORMATEX*)fmt, nullptr);
		}
		block = fmt->Format.nBlockAlign;
		PropVariantClear(&var);
		prop->Release();
	}

	// �I�[�f�B�I�C�x���g�̃Z�b�g
	void* audioEvent = nullptr;
	{
		audioEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		hr = audio->SetEventHandle(audioEvent);
	}

	// �I�[�f�B�I�����_���̏���
	IAudioRenderClient* render = nullptr;
	{
		hr = audio->GetService(IID_PPV_ARGS(&render));

		//�o�b�t�@�̃N���A
		unsigned __int32 fream = 0;
		hr = audio->GetBufferSize(&fream);
		unsigned char* data = nullptr;
		hr = render->GetBuffer(fream, &data);
		hr = render->ReleaseBuffer(fream, AUDCLNT_BUFFERFLAGS_SILENT);
	}

	// �X���b�h�̏���
	unsigned __int32 fream = 0;
	hr = audio->GetBufferSize(&fream);
	std::vector<float>a(fream * block / sizeof(float));
	for (unsigned int i = 0; i < a.size(); i += block / sizeof(float))
	{
		for (unsigned int c = 0; c < block / sizeof(float); ++c)
		{
			a[i + c] = 2.0f * std::sin(2.0f * 3.14159265f * 440.0f * i / 48000);
		}
	}
	void* threadEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	void* wait[] = {
		audioEvent,
		threadEvent
	};
	std::thread th = std::thread([&]()->void {
		while (true)
		{
			auto hr = WaitForMultipleObjects(_countof(wait), wait, false, INFINITE);
			if (hr != WAIT_OBJECT_0)
			{
				break;
			}

			unsigned __int32 fream = 0;
			hr = audio->GetBufferSize(&fream);
			unsigned char* data = nullptr;
			hr = render->GetBuffer(fream, &data);
			memcpy(data, a.data(), sizeof(a[0])* a.size());
			hr = render->ReleaseBuffer(fream, 0);
		}
	});

	// �I�[�f�B�I�����_�����O�J�n
	{
		hr = audio->Start();
	}

	while (true)
	{

	}

	SetEvent(threadEvent);
	th.join();
	CloseHandle(threadEvent);
	CloseHandle(audioEvent);
	render->Release();
	audio->Release();
	device->Release();

	CoUninitialize();

	return 0;
}