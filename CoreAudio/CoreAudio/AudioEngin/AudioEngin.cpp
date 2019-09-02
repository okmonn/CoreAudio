#include "AudioEngin.h"
#include <ks.h>
#include <ksmedia.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <propvarutil.h>
#include <Audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include "..//etc/WavFmt.h"

#pragma comment(lib, "Propsys.lib")

// スピーカ一覧
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

// コンストラクタ
AudioEngin::AudioEngin(const unsigned int& index, const okmonn::DeviceType& type) :
	device(nullptr), audio(nullptr), render(nullptr), deviceType(type)
{
	for (void*& i : handle)
	{
		i = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	}

	CreateDevice(index, type);

	wav::Load("_rsc/SOS.wav", waveInfo, wave);
}

// デストラクタ
AudioEngin::~AudioEngin()
{
	if (th.joinable() == true)
	{
		SetEvent(handle[1]);
		th.join();
	}
	for (void*& i : handle)
	{
		CloseHandle(i);
	}
}

// エンドポイントデバイス生成
void AudioEngin::CreateDevice(const unsigned int& index, const okmonn::DeviceType& type)
{
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	hr = enumerator->EnumAudioEndpoints(EDataFlow::eRender, DEVICE_STATE_ACTIVE, &collection);
	_ASSERT(hr == S_OK);

	unsigned int num = 0;
	hr = collection->GetCount(&num);
	_ASSERT(hr == S_OK);

	if (num <= index)
	{
		hr = enumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eConsole, &device);
		_ASSERT(hr == S_OK);
	}
	else
	{
		hr = collection->Item(index, &device);
		_ASSERT(hr == S_OK);
	}
}

// オーディオクライアント生成
long AudioEngin::CreateAudio(const okmonn::AudioType& type, void* addr)
{
	WAVEFORMATEXTENSIBLE* fmt = (WAVEFORMATEXTENSIBLE*)addr;
	WAVEFORMATEXTENSIBLE* tmp = nullptr;
	
	auto hr = audio->IsFormatSupported(_AUDCLNT_SHAREMODE(type), (WAVEFORMATEX*)fmt, (WAVEFORMATEX**)&tmp);
	if (FAILED(hr))
	{
		return hr;
	}

	//レイテンシ取得
	REFERENCE_TIME defPeriod = 0;
	REFERENCE_TIME minPeriod = 0;
	hr = audio->GetDevicePeriod(&defPeriod, &minPeriod);
	_ASSERT(hr == S_OK);

	//初期化
	hr = audio->Initialize(_AUDCLNT_SHAREMODE(type), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
		defPeriod, defPeriod * int(type), (WAVEFORMATEX*)fmt, nullptr);
	if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
	{
		//修正後のフレーム数を取得
		unsigned __int32 fream = 0;
		hr = audio->GetBufferSize(&fream);
		defPeriod = REFERENCE_TIME(std::round(10000.0 * 1000 * fream / fmt->Format.nSamplesPerSec));

		//再挑戦
		hr = audio->Initialize(_AUDCLNT_SHAREMODE(type), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
			defPeriod, defPeriod * int(type), (WAVEFORMATEX*)fmt, nullptr);
	}
	if (FAILED(hr))
	{
		return hr;
	}

	//イベントセット
	hr = audio->SetEventHandle(handle[0]);
	if (!FAILED(hr))
	{
		audioType = type;
	}
	
	return hr;
}

// オーディオレンダラー生成
long AudioEngin::CreateRender(void)
{
	auto hr = audio->GetService(IID_PPV_ARGS(&render));
	if (FAILED(hr))
	{
		return hr;
	}

	//バッファのクリア
	unsigned __int32 fream = 0;
	hr = audio->GetBufferSize(&fream);
	_ASSERT(hr == S_OK);

	unsigned char* data = nullptr;
	hr = render->GetBuffer(fream, &data);
	_ASSERT(hr == S_OK);

	hr = render->ReleaseBuffer(fream, AUDCLNT_BUFFERFLAGS_SILENT);
	_ASSERT(hr == S_OK);

	return hr;
}

// 初期化
long AudioEngin::Initialize(const okmonn::AudioType& type, const okmonn::Info* info)
{
	auto hr = device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, (void**)& audio);
	if (FAILED(hr))
	{
		return hr;
	}

	WAVEFORMATEXTENSIBLE* fmt = nullptr;
	if (info == nullptr)
	{
		if (type == okmonn::AudioType::EXCLUSIVE)
		{
			//フォーマットのチェック
			Microsoft::WRL::ComPtr<IPropertyStore>prop = nullptr;
			hr = device->OpenPropertyStore(STGM_READ, &prop);
			_ASSERT(hr == S_OK);

			PROPVARIANT var{};
			PropVariantInit(&var);
			hr = prop->GetValue(PKEY_AudioEngine_DeviceFormat, &var);
			_ASSERT(hr == S_OK);

			fmt = (WAVEFORMATEXTENSIBLE*)var.blob.pBlobData;
		}
		else
		{
			hr = audio->GetMixFormat((WAVEFORMATEX * *)& fmt);
			_ASSERT(hr == S_OK);
		}
	}
	else
	{
		fmt = new WAVEFORMATEXTENSIBLE();
		fmt->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		fmt->Format.nSamplesPerSec = info->sample;
		fmt->Format.wBitsPerSample = info->bit;
		fmt->Format.nChannels = info->channel;
		fmt->Format.nBlockAlign = fmt->Format.nChannels * fmt->Format.wBitsPerSample / 8;
		fmt->Format.nAvgBytesPerSec = fmt->Format.nSamplesPerSec * fmt->Format.nBlockAlign;
		fmt->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

		fmt->dwChannelMask = spk[fmt->Format.nChannels - 1];
		fmt->Samples.wValidBitsPerSample = fmt->Format.wBitsPerSample;
		fmt->SubFormat = (info->floating == false) ? KSDATAFORMAT_SUBTYPE_PCM : KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	hr = CreateAudio(type, (void*)fmt);
	_ASSERT(hr == S_OK);

	if (deviceType == okmonn::DeviceType::RENDER)
	{
		hr = CreateRender();
	}

	if (!FAILED(hr))
	{
		this->info = okmonn::Info(unsigned short(fmt->Format.nSamplesPerSec), unsigned char(fmt->Format.wBitsPerSample), unsigned char(fmt->Format.nChannels),
			(fmt->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) ? true : false);

		th = std::thread(&AudioEngin::Stream, this);
	}

	return hr;
}

// 非同期処理
void AudioEngin::Stream(void)
{

	long read = 0;

	unsigned __int32 fream = 0;
	auto hr = audio->GetBufferSize(&fream);
	_ASSERT(hr == S_OK);

	unsigned __int32 padding = 0;
	unsigned char* data = nullptr;
	while (true)
	{
		hr = WaitForMultipleObjects(handle.size(), handle.data(), false, INFINITE);
		if (hr != WAIT_OBJECT_0)
		{
			break;
		}

		if (audioType == okmonn::AudioType::SHARED)
		{
			hr = audio->GetCurrentPadding(&padding);
			_ASSERT(hr == S_OK);
		}

		hr = render->GetBuffer(fream - padding, &data);
		_ASSERT(hr == S_OK);


		std::vector<float>tmp((fream - padding) * info.channel);
		short* addr = (short*)&wave->at(read);
		for (unsigned int i = 0; i < tmp.size(); ++i)
		{
			tmp[i] = float(addr[i]) / float(0xffff / 2);
			//tmp[i] = addr[i] * 0xffff;
		}
		memcpy(data, tmp.data(), (fream - padding) * info.channel * info.bit / 8);

		hr = render->ReleaseBuffer(fream - padding, 0);
		_ASSERT(hr == S_OK);

		read += (fream - padding) * info.channel * waveInfo.bit / 8;
		if (read >= wave->size())
		{
			read = 0;
		}
	}
}

// エンジン開始
long AudioEngin::Start(void)
{
	return audio->Start();
}

// エンジン停止
long AudioEngin::Stop(void)
{
	return audio->Stop();
}
