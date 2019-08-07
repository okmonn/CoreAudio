#include "AudioEngin/AudioEngin.h"
#include "Function/Function.h"
#include <Windows.h>
#include <iostream>

int main(void)
{
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//�G���h�|�C���g�f�o�C�X�̗�
	{
		auto prop = okmonn::GetDeviceProp(okmonn::DeviceType::RENDER);
		for (size_t i = 0; i < prop.size(); ++i)
		{
			std::cout << i << std::endl;
			std::cout << prop[i].id << std::endl;
			std::cout << prop[i].state << std::endl;
			std::cout << prop[i].fullName << std::endl;
			std::cout << prop[i].shortName << std::endl;
			std::cout << prop[i].deviceName << std::endl;
		}
	}

	Microsoft::WRL::ComPtr<AudioEngin>audio = nullptr;
	okmonn::CreateAudioEngin(DEFAULT_DEVICE, okmonn::DeviceType::RENDER, IID_PPV_ARGS(&audio));
	audio->Initialize(okmonn::AudioType::SHARED);
	audio->Start();

	while (true)
	{

	}

	// �f�t�H���g�G���h�|�C���g�f�o�C�X�Z�b�g
	/*IMMDevice* device = nullptr;
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
		{
			*fmt = {};
			fmt->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
			fmt->Format.nSamplesPerSec = info.sample;
			fmt->Format.wBitsPerSample = info.bit;
			fmt->Format.nChannels = info.channel;
			fmt->Format.nBlockAlign = fmt->Format.nChannels * fmt->Format.wBitsPerSample / 8;
			fmt->Format.nAvgBytesPerSec = fmt->Format.nSamplesPerSec * fmt->Format.nBlockAlign;
			fmt->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

			fmt->dwChannelMask = spk[fmt->Format.nChannels - 1];
			fmt->Samples.wValidBitsPerSample = 32;
			fmt->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		}
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
	void* threadEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	void* wait[] = {
		audioEvent,
		threadEvent
	};
	std::thread th = std::thread([&]()->void {
		unsigned long read = 0;
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

			std::vector<int>tmp(fream* info.channel);
			short* addr = (short*)&wave->at(read);
			for (unsigned int i = 0; i < tmp.size(); ++i)
			{
				tmp[i] = addr[i] * 0xffff;
			}
			memcpy(data, &tmp.at(0), fream * block);

			hr = render->ReleaseBuffer(fream, 0);
			read += fream * 4;
			if (read >= wave->size())
			{
				read = 0;
			}
		}
	});

	// �I�[�f�B�I�����_�����O�J�n
	{
		hr = audio->Start();
	}

	while (!(GetKeyState(VK_ESCAPE) & 0x80))
	{

	}

	SetEvent(threadEvent);
	th.join();
	CloseHandle(threadEvent);
	CloseHandle(audioEvent);
	render->Release();
	audio->Release();
	device->Release();*/

	CoUninitialize();

	return 0;
}