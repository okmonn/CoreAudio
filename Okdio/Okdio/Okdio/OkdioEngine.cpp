#include "OkdioEngine.h"
#include "Okdio.h"
#include "SoundBuffer/SoundBuffer.h"
#include "OkdioList/OkdioList.h"
#include <ks.h>
#include <vector>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

#pragma comment(lib, "Propsys.lib")

// �X�s�[�J�[�ꗗ
const unsigned long spk[] = {
	KSAUDIO_SPEAKER_MONO,
	KSAUDIO_SPEAKER_STEREO,
	KSAUDIO_SPEAKER_STEREO | SPEAKER_LOW_FREQUENCY,
	KSAUDIO_SPEAKER_QUAD,
	0,
	KSAUDIO_SPEAKER_5POINT1,
	0,
	KSAUDIO_SPEAKER_7POINT1_SURROUND
};

// �R���X�g���N�^
OkdioEngine::OkdioEngine(const okmonn::AudioDevType& devType, const okmonn::AudioType& audioType, const int& devIndex) :
	devType(devType), audioType(audioType), dev(nullptr), audio(nullptr), render(nullptr)
{
	for (void*& i : handle)
	{
		i = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	}

	CreateDevice(devIndex);
	CreateAudio();
	Initialized();
	CreateRender();
	if (th.joinable() == false)
	{
		th = std::thread(&OkdioEngine::Stream, this);
	}
}

// �f�X�g���N�^
OkdioEngine::~OkdioEngine()
{
	if (th.joinable() == true)
	{
		SetEvent(handle[1]);
		th.join();
	}
	for (void*& i : handle)
	{
		if (i != nullptr)
		{
			CloseHandle(i);
		}
	}
}

// Okdio����
bool OkdioEngine::CreateOkdio(const std::string& fileName, const GUID& id, void** okdio)
{
	if (id == __uuidof(Okdio))
	{
		*okdio = new Okdio(fileName);
		OkdioList::Get().AddList((Okdio**)okdio);
		return true;
	}

	return false;
}

// �I�[�f�B�I�f�o�C�X����
void OkdioEngine::CreateDevice(const int& devIndex)
{
	//�I�[�f�B�I�f�o�C�X��
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
	_ASSERT(hr == S_OK);

	//�I�[�f�B�I�f�o�C�X�R���N�V����
	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	hr = enumerator->EnumAudioEndpoints(EDataFlow(devType), DEVICE_STATE_ACTIVE, &collection);
	_ASSERT(hr == S_OK);

	//�f�o�C�X��
	unsigned int devNum = 0;
	hr = collection->GetCount(&devNum);
	_ASSERT(hr == S_OK);

	//�f�t�H���g�f�o�C�X�g�p
	if (devIndex < 0)
	{
		auto hr = enumerator->GetDefaultAudioEndpoint(EDataFlow(devType), ERole::eConsole, &dev);
	}
	//�w��f�o�C�X�g�p
	else
	{
		auto hr = collection->Item(devIndex, &dev);
	}
	_ASSERT(hr == S_OK);
}

// �I�[�f�B�I�N���C�A���g����
void OkdioEngine::CreateAudio(void)
{
	auto hr = dev->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, (void**)&audio);
	_ASSERT(hr == S_OK);
}

// �t�H�[�}�b�g�擾
void* OkdioEngine::GetFormat(void)
{
	WAVEFORMATEXTENSIBLE* fmt = nullptr;
	//���L���[�h
	if (audioType == okmonn::AudioType::shared)
	{
		auto hr = audio->GetMixFormat((WAVEFORMATEX**)&fmt);
		_ASSERT(hr == S_OK);
	}
	//�r�����[�h
	else
	{
		Microsoft::WRL::ComPtr<IPropertyStore>store = nullptr;
		auto hr = dev->OpenPropertyStore(STGM_READ, &store);
		_ASSERT(hr == S_OK);

		PROPVARIANT variant{};
		hr = store->GetValue(PKEY_AudioEngine_DeviceFormat, &variant);
		_ASSERT(hr == S_OK);

		fmt = (WAVEFORMATEXTENSIBLE*)variant.blob.pBlobData;
	}

	return fmt;
}

// �I�[�f�B�I�N���C�A���g������
void OkdioEngine::Initialized(void)
{
	//�t�H�[�}�b�g�`�F�b�N
	WAVEFORMATEXTENSIBLE* fmt = (WAVEFORMATEXTENSIBLE*)GetFormat();
	WAVEFORMATEXTENSIBLE* corre = nullptr;
	auto hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)fmt, (WAVEFORMATEX**)&corre);
	if (SUCCEEDED(hr))
	{
		this->info.sample  = fmt->Format.nSamplesPerSec;
		this->info.byte    = unsigned char(fmt->Format.wBitsPerSample / 8);
		this->info.channel = unsigned char(fmt->Format.nChannels);
		this->info.flag    = (fmt->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) ? 0 : 1;
	}
	else
	{
		hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)fmt, nullptr);
		_ASSERT(hr == S_OK);

		this->info.sample  = fmt->Format.nSamplesPerSec;
		this->info.byte    = unsigned char(fmt->Format.wBitsPerSample / 8);
		this->info.channel = unsigned char(fmt->Format.nChannels);
		this->info.flag    = (fmt->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) ? 0 : 1;
	}

	//���C�e���V�擾
	REFERENCE_TIME def = 0;
	REFERENCE_TIME min = 0;
	hr = audio->GetDevicePeriod(&def, &min);
	_ASSERT(hr == S_OK);

	//������
	hr = audio->Initialize(AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
		def, def * int(audioType), (WAVEFORMATEX*)fmt, nullptr);
	//�Ē���
	if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
	{
		//�t���[���擾
		unsigned __int32 frame = 0;
		hr = audio->GetBufferSize(&frame);
		_ASSERT(hr == S_OK);

		def = REFERENCE_TIME(std::round(10000.0 * 1000.0 * frame / fmt->Format.nSamplesPerSec));
		hr = audio->Initialize(AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
			def, def * int(audioType), (WAVEFORMATEX*)fmt, nullptr);
	}
	_ASSERT(hr == S_OK);

	hr = audio->SetEventHandle(handle[0]);
	_ASSERT(hr == S_OK);
}

// �I�[�f�B�I�����_���[����
void OkdioEngine::CreateRender(void)
{
	auto hr = audio->GetService(IID_PPV_ARGS(&render));
	_ASSERT(hr == S_OK);

	//�����o�b�t�@�N���A
	unsigned __int32 frame = 0;
	hr = audio->GetBufferSize(&frame);
	_ASSERT(hr == S_OK);
	unsigned char* buf = nullptr;
	hr = render->GetBuffer(frame, &buf);
	_ASSERT(hr == S_OK);
	hr = render->ReleaseBuffer(frame, AUDCLNT_BUFFERFLAGS_SILENT);
	_ASSERT(hr == S_OK);
}

// �J�n
bool OkdioEngine::Start(void)
{
	auto hr = audio->Start();
	if (SUCCEEDED(hr))
	{
		return true;
	}

	return false;
}

// ��~
bool OkdioEngine::Stop(void)
{
	auto hr = audio->Stop();
	if (SUCCEEDED(hr))
	{
		return true;
	}

	return false;
}

// �񓯊�����
void OkdioEngine::Stream(void)
{
	unsigned __int32 frame = 0;
	auto hr = audio->GetBufferSize(&frame);
	_ASSERT(hr == S_OK);

	unsigned __int32 padding = 0;
	unsigned char* buf = nullptr;
	while (true)
	{
		hr = WaitForMultipleObjects(handle.size(), handle.data(), false, INFINITE);
		if (hr != WAIT_OBJECT_0)
		{
			break;
		}

		//���L���[�h�̂�
		if (audioType == okmonn::AudioType::shared)
		{
			hr = audio->GetCurrentPadding(&padding);
			_ASSERT(hr == S_OK);
		}

		hr = render->GetBuffer(frame - padding, &buf);
		_ASSERT(hr == S_OK);

		std::unique_lock<std::mutex>lock(mtx);

		//�o�b�t�@�ɏ悹�鐔
		unsigned int num = (frame - padding) * info.channel;
		SoundBuffer::Get().ReSize(num);
		for (auto& i : OkdioList::Get().GetList())
		{
			i->Submit(num);
		}

		//�f�[�^�R�s�[
		std::weak_ptr<std::vector<float>>wave = SoundBuffer::Get().GetWave();
		memcpy(buf, wave.lock()->data(), num * info.byte);

		hr = render->ReleaseBuffer(frame - padding, 0);
		_ASSERT(hr == S_OK);

		//�o�b�t�@�N���A
		SoundBuffer::Get().Clear();
	}
}
 