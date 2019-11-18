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

// スピーカー一覧
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

// デストラクタ
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

// Okdio生成
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

// オーディオデバイス生成
void OkdioEngine::CreateDevice(const int& devIndex)
{
	//オーディオデバイス列挙
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
	_ASSERT(hr == S_OK);

	//オーディオデバイスコレクション
	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	hr = enumerator->EnumAudioEndpoints(EDataFlow(devType), DEVICE_STATE_ACTIVE, &collection);
	_ASSERT(hr == S_OK);

	//デバイス数
	unsigned int devNum = 0;
	hr = collection->GetCount(&devNum);
	_ASSERT(hr == S_OK);

	//デフォルトデバイス使用
	if (devIndex < 0)
	{
		auto hr = enumerator->GetDefaultAudioEndpoint(EDataFlow(devType), ERole::eConsole, &dev);
	}
	//指定デバイス使用
	else
	{
		auto hr = collection->Item(devIndex, &dev);
	}
	_ASSERT(hr == S_OK);
}

// オーディオクライアント生成
void OkdioEngine::CreateAudio(void)
{
	auto hr = dev->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, (void**)&audio);
	_ASSERT(hr == S_OK);
}

// フォーマット取得
void* OkdioEngine::GetFormat(void)
{
	WAVEFORMATEXTENSIBLE* fmt = nullptr;
	//共有モード
	if (audioType == okmonn::AudioType::shared)
	{
		auto hr = audio->GetMixFormat((WAVEFORMATEX**)&fmt);
		_ASSERT(hr == S_OK);
	}
	//排他モード
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

// オーディオクライアント初期化
void OkdioEngine::Initialized(void)
{
	//フォーマットチェック
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

	//レイテンシ取得
	REFERENCE_TIME def = 0;
	REFERENCE_TIME min = 0;
	hr = audio->GetDevicePeriod(&def, &min);
	_ASSERT(hr == S_OK);

	//初期化
	hr = audio->Initialize(AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
		def, def * int(audioType), (WAVEFORMATEX*)fmt, nullptr);
	//再挑戦
	if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
	{
		//フレーム取得
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

// オーディオレンダラー生成
void OkdioEngine::CreateRender(void)
{
	auto hr = audio->GetService(IID_PPV_ARGS(&render));
	_ASSERT(hr == S_OK);

	//初期バッファクリア
	unsigned __int32 frame = 0;
	hr = audio->GetBufferSize(&frame);
	_ASSERT(hr == S_OK);
	unsigned char* buf = nullptr;
	hr = render->GetBuffer(frame, &buf);
	_ASSERT(hr == S_OK);
	hr = render->ReleaseBuffer(frame, AUDCLNT_BUFFERFLAGS_SILENT);
	_ASSERT(hr == S_OK);
}

// 開始
bool OkdioEngine::Start(void)
{
	auto hr = audio->Start();
	if (SUCCEEDED(hr))
	{
		return true;
	}

	return false;
}

// 停止
bool OkdioEngine::Stop(void)
{
	auto hr = audio->Stop();
	if (SUCCEEDED(hr))
	{
		return true;
	}

	return false;
}

// 非同期処理
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

		//共有モードのみ
		if (audioType == okmonn::AudioType::shared)
		{
			hr = audio->GetCurrentPadding(&padding);
			_ASSERT(hr == S_OK);
		}

		hr = render->GetBuffer(frame - padding, &buf);
		_ASSERT(hr == S_OK);

		std::unique_lock<std::mutex>lock(mtx);

		//バッファに乗せる数
		unsigned int num = (frame - padding) * info.channel;
		SoundBuffer::Get().ReSize(num);
		for (auto& i : OkdioList::Get().GetList())
		{
			i->Submit(num);
		}

		//データコピー
		std::weak_ptr<std::vector<float>>wave = SoundBuffer::Get().GetWave();
		memcpy(buf, wave.lock()->data(), num * info.byte);

		hr = render->ReleaseBuffer(frame - padding, 0);
		_ASSERT(hr == S_OK);

		//バッファクリア
		SoundBuffer::Get().Clear();
	}
}
 