#include "Okdio.h"
#include "SoundLoader/SoundLoader.h"
#include <ks.h>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

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
OkdioEngine::OkdioEngine(const okmonn::AudioDeviceType& devType, const okmonn::AudioType& audioType, const char& deviceIndex) :
	devType(devType), audioType(audioType)
{
	for (void*& i : handle)
	{
		i = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		_ASSERT(i != nullptr);
	}

	CreateDevice(deviceIndex);
	CreateAudioClient();
	Initialize();
	CreateAudioRender();
	SoundLoader::Get().SetOutInfo(info);
	th = std::thread(&OkdioEngine::Stream, this);
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
		CloseHandle(i);
	}
}

// Okdio生成
long OkdioEngine::CreateOkdio(const std::string& fileName, const GUID& id, void** obj)
{
	if (obj == nullptr)
	{
		return E_INVALIDARG;
	}

	*obj = nullptr;
	if (id == __uuidof(Okdio))
	{
		*obj = new Okdio(fileName);
		okdio.push_back((Okdio*)* obj);
	}
	else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

// オーディオデバイス生成
void OkdioEngine::CreateDevice(const char& deviceIndex)
{
	long hr = S_OK;

	//オーディオデバイス列挙用インターフェース
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
		_ASSERT(hr == S_OK);
	}

	//オーディオデバイスのコレクション列挙用インターフェース
	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	{
		hr = enumerator->EnumAudioEndpoints(EDataFlow(audioType), DEVICE_STATE_ACTIVE, &collection);
		_ASSERT(hr == S_OK);
	}

	//コレクション数取得
	unsigned int deviceNum = 0;
	{
		hr = collection->GetCount(&deviceNum);
		_ASSERT(hr == S_OK);
	}

	//デフォルトデバイス使用
	if (deviceIndex < 0 || deviceNum <= unsigned int(deviceIndex))
	{
		hr = enumerator->GetDefaultAudioEndpoint(EDataFlow(audioType), ERole::eConsole, &device);
		_ASSERT(hr == S_OK);
	}
	//指定デバイス使用
	else
	{
		hr = collection->Item(deviceIndex, &device);
		_ASSERT(hr == S_OK);
	}
}

// フォーマット設定
void* OkdioEngine::SetFormat(IAudioClient* audio, const okmonn::AudioInfo* info)
{
	long hr = S_OK;

	WAVEFORMATEXTENSIBLE* fmt = nullptr;
	//デバイスのフォーマットを設定
	if (info == nullptr)
	{
		//共有モード
		if (audioType == okmonn::AudioType::SHARED)
		{
			hr = audio->GetMixFormat((WAVEFORMATEX * *)& fmt);
			_ASSERT(hr == S_OK);
		}
		//排他モード
		else
		{
			Microsoft::WRL::ComPtr<IPropertyStore>prop = nullptr;
			hr = device->OpenPropertyStore(STGM_READ, &prop);
			_ASSERT(hr == S_OK);

			PROPVARIANT var{};
			PropVariantInit(&var);
			hr = prop->GetValue(PKEY_AudioEngine_DeviceFormat, &var);
			_ASSERT(hr == S_OK);

			fmt = (WAVEFORMATEXTENSIBLE*)var.blob.pBlobData;
		}
	}
	//指定したフォーマットを設定
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
		fmt->SubFormat = (info->flag == 0) ? KSDATAFORMAT_SUBTYPE_PCM : KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	return fmt;
}

// オーディオクライアント生成
void OkdioEngine::CreateAudioClient(void)
{
	//オーディオクライアント生成
	auto hr = device->Activate(__uuidof(audio), CLSCTX_INPROC_SERVER, nullptr, (void**)& audio);
	_ASSERT(hr == S_OK);

	//フォーマットのチェック
	WAVEFORMATEXTENSIBLE* fmt = (WAVEFORMATEXTENSIBLE*)SetFormat(audio.Get());
	WAVEFORMATEXTENSIBLE* corre = nullptr;
	{
		hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)fmt, (WAVEFORMATEX * *)& corre);
		if (hr == S_OK)
		{
			info.sample = fmt->Format.nSamplesPerSec;
			info.bit = unsigned char(fmt->Format.wBitsPerSample);
			info.channel = unsigned char(fmt->Format.nChannels);
			info.flag = (fmt->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) ? 0 : 1;
		}
		else
		{
			OutputDebugStringA("\nフォーマットがサポートされていません\n最適なフォーマットに修正します\n");

			info.sample = corre->Format.nSamplesPerSec;
			info.bit = unsigned char(corre->Format.wBitsPerSample);
			info.channel = unsigned char(corre->Format.nChannels);
			info.flag = (corre->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) ? 0 : 1;

			hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)corre, nullptr);
			_ASSERT(hr == S_OK);
		}
	}
}

// オーディオクライアント初期化
void OkdioEngine::Initialize(void)
{
	long hr = S_OK;

	//レイテンシ取得
	REFERENCE_TIME defPeriod = 0;
	REFERENCE_TIME minPeriod = 0;
	{
		hr = audio->GetDevicePeriod(&defPeriod, &minPeriod);
		_ASSERT(hr == S_OK);
	}

	WAVEFORMATEXTENSIBLE fmt{};
	fmt.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	fmt.Format.nSamplesPerSec = info.sample;
	fmt.Format.wBitsPerSample = info.bit;
	fmt.Format.nChannels = info.channel;
	fmt.Format.nBlockAlign = fmt.Format.nChannels * fmt.Format.wBitsPerSample / 8;
	fmt.Format.nAvgBytesPerSec = fmt.Format.nSamplesPerSec * fmt.Format.nBlockAlign;
	fmt.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

	fmt.dwChannelMask = spk[fmt.Format.nChannels - 1];
	fmt.Samples.wValidBitsPerSample = fmt.Format.wBitsPerSample;
	fmt.SubFormat = (info.flag == 0) ? KSDATAFORMAT_SUBTYPE_PCM : KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

	//初期化
	hr = audio->Initialize(AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
		defPeriod, defPeriod * int(audioType), (WAVEFORMATEX*)& fmt, nullptr);
	//アライメントされていない場合
	if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
	{
		//修正後のフレーム数を取得
		unsigned __int32 fream = 0;
		hr = audio->GetBufferSize(&fream);
		_ASSERT(hr == S_OK);
		defPeriod = REFERENCE_TIME(std::round(10000.0 * 1000 * fream / fmt.Format.nSamplesPerSec));

		//再挑戦
		hr = audio->Initialize(_AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
			defPeriod, defPeriod * int(audioType), (WAVEFORMATEX*)& fmt, nullptr);
	}
	_ASSERT(hr == S_OK);

	//イベントセット
	hr = audio->SetEventHandle(handle[0]);
	_ASSERT(hr == S_OK);
}

// オーディオレンダラー生成
void OkdioEngine::CreateAudioRender(void)
{
	auto hr = audio->GetService(IID_PPV_ARGS(&render));
	_ASSERT(hr == S_OK);

	//初期バッファクリア
	{
		unsigned __int32 fream = 0;
		hr = audio->GetBufferSize(&fream);
		_ASSERT(hr == S_OK);

		unsigned char* data = nullptr;
		hr = render->GetBuffer(fream, &data);
		_ASSERT(hr == S_OK);

		hr = render->ReleaseBuffer(fream, AUDCLNT_BUFFERFLAGS_SILENT);
		_ASSERT(hr == S_OK);
	}
}

std::vector<float> Test(const std::vector<double>& corre, const okmonn::ConvertParam& sample, const std::vector<float>& data, const okmonn::AudioInfo& info)
{
	std::vector<std::vector<float>>channel(info.channel);
	for (size_t ch = 0; ch < channel.size(); ++ch)
	{
		channel[ch].resize((data.size() / info.channel) * sample.rate);
		for (size_t i = 0; i < data.size() / info.channel; ++i)
		{
			channel[ch][i * sample.rate] = data[(i * info.channel) + ch];
			for (unsigned char n = 1; n < sample.rate; ++n)
			{
				channel[ch][(i * sample.rate) + n] = 0.0f;
			}
		}
	}

	std::vector<float>convert;
	double start = 0;
	double end = corre.size() - 1;
	double gap = sample.gap;
	unsigned int target = end / 2;
	while (end < channel[0].size())
	{
		if (gap >= 1.0)
		{
			start += 1.0;
			end += 1.0;
			gap -= 1.0;
		}
		else
		{
			std::vector<double>tmp(corre.size() - 1, 0);
			for (size_t i = 0; i < tmp.size(); ++i)
			{
				tmp[i] = ((corre[i + 1] - corre[i]) * (1.0 - gap)) + corre[i];
			}

			tmp[tmp.size() / 2 - 1] = ((corre[(tmp.size() / 2) - 1] - corre[tmp.size() / 2]) * std::pow(-gap, 2.0)) + corre[tmp.size() / 2];
			tmp[tmp.size() / 2 + 0] = ((corre[(tmp.size() / 2) - 1] - corre[tmp.size() / 2]) * std::pow((1.0 - gap), 2.0)) + corre[tmp.size() / 2];

			for (auto& i : channel)
			{
				double val = 0.0;
				for (size_t n = 0; n < tmp.size(); ++n)
				{
					val += i[start + n] * tmp[n];
				}
				convert.push_back(float(val));
			}
			gap += sample.gap;
		}
	}

	return convert;
}

std::vector<float> Test2(const std::vector<double>& corre, const okmonn::ConvertParam& sample, const std::vector<float>& data, const okmonn::AudioInfo& info)
{
	std::vector<float>convert(data.size() * (double(sample.sample) / double(info.sample)), 0);
	unsigned int offset = (corre.size() - 1) / 2;
	unsigned int index = 0;
	double gap = sample.gap;
	unsigned int a = 0;
	while (index < convert.size())
	{
		double integer = 0.0;
		gap = std::modf(gap, &integer);
		offset += int(integer);
		//変換
		for (size_t i = 0; i < (corre.size() - 1); ++i)
		{
			long tmp = offset - ((corre.size() - 1) / 2) + i;
			if (size_t(tmp / sample.rate) * info.channel >= data.size())
			{
				break;
			}

			if (tmp % sample.rate == 0)
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
					if ((tmp / sample.rate) * info.channel + ch < data.size())
					{
						a = (tmp / sample.rate) * info.channel + ch;
						convert[index + ch] += data[(tmp / sample.rate) * info.channel + ch] * float(comp);
					}
				}
			}
		}

		//ゲイン調節
		for (unsigned char ch = 0; ch < info.channel; ++ch)
		{
			convert[index + ch] *= sample.rate - 0.2f;
		}
		index += info.channel;
		gap += sample.gap;
	}

	return convert;
}

#include <cuda_runtime.h>
__global__ void Kernel(float* buf, short* data, double* corre)
{

}

#include "SoundLoader/SoundLoader.h"

// 非同期処理
void OkdioEngine::Stream(void)
{
	std::string name = "Demo1.wav";
	auto q = SoundLoader::Get().Load(name);
	auto wave1 = SoundLoader::Get().GetWave(name);
	auto waveInfo = SoundLoader::Get().GetInfo(name);
	std::vector<float>wave2(wave1->size() / (waveInfo.bit / 8));
	short* ptr = (short*)wave1->data();
	for (size_t i = 0; i < wave2.size(); ++i)
	{
		wave2[i] = okmonn::Normalize<float>(ptr[i]);
	}
	unsigned int index = 0;
	auto sample = okmonn::GetConvertParam(waveInfo.sample, 48000);
	auto de = okmonn::GetDegree(100, sample);
	auto corre = okmonn::Sinc(100, de, sample);
	wave2 = Test2(corre, sample, wave2, waveInfo);

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


		//データのコピー
		unsigned int size = (fream - padding) * info.channel;
		if (index + size >= wave2.size())
		{
			size = wave2.size() - index;
		}
		memcpy(data, &wave2[index], sizeof(wave2[0]) * size);

		hr = render->ReleaseBuffer(fream - padding, 0);
		_ASSERT(hr == S_OK);

		index += (fream - padding) * info.channel;
		if (wave2.size() <= index)
		{
			index = 0;
		}
	}

	/*unsigned __int32 fream = 0;
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

		//データのコピー
		std::vector<float>wave((fream - padding) * info.channel, 0);
		TTT(wave);

		memcpy(data, wave.data(), sizeof(wave[0]) * wave.size());

		hr = render->ReleaseBuffer(fream - padding, 0);
		_ASSERT(hr == S_OK);
	}*/
}

void OkdioEngine::TTT(std::vector<float>& data)
{
	for (auto itr = okdio.begin(); itr != okdio.end();)
	{
		std::unique_lock<std::mutex>lock(mtx);
		if ((*itr) != nullptr)
		{
			auto corre = (*itr)->GetConvertCorre().lock();
			short* ptr = (short*)(*itr)->GetWave().lock()->data();

			bool flag = false;
			unsigned int index = 0;
			while (index < data.size())
			{
				double integer = 0.0;
				(*itr)->gap = std::modf((*itr)->gap, &integer);
				(*itr)->offset += int(integer);

				unsigned int a = 0;
				for (size_t i = 0; i < corre->size() - 1; ++i)
				{
					long tmp = (*itr)->offset - ((corre->size()) / 2) + i;
					if (tmp < 0)
					{
						continue;
					}
					if (size_t(tmp / (*itr)->GetConvertParam().rate) * info.channel >= (*itr)->GetWaveNum())
					{
						(*itr)->offset = 0;
						(*itr)->gap = (*itr)->GetConvertParam().gap;
						flag = true;
						break;
					}

					if (tmp % (*itr)->GetConvertParam().rate == 0)
					{
						++a;
						double comp = ((corre->at(i + 1) - corre->at(i)) * (1.0 - (*itr)->gap)) + corre->at(i);
						if (i == ((corre->size() - 1) / 2) - 1)
						{
							comp = ((corre->at(((corre->size() - 1) / 2) - 1) - corre->at(((corre->size() - 1) / 2))) * std::pow(-(*itr)->gap, 2.0)) + corre->at(((corre->size() - 1) / 2));
						}
						else if (i == (corre->size() - 1) / 2)
						{
							comp = ((corre->at(((corre->size() - 1) / 2) - 1) - corre->at(((corre->size() - 1) / 2))) * std::pow((1.0 - (*itr)->gap), 2.0)) + corre->at(((corre->size() - 1) / 2));
						}

						for (unsigned char ch = 0; ch < info.channel; ++ch)
						{
							if ((tmp / (*itr)->GetConvertParam().rate) * info.channel + ch < (*itr)->GetWaveNum())
							{
								data[index + ch] += okmonn::Normalize<float>(ptr[(tmp / (*itr)->GetConvertParam().rate) * info.channel + ch]) * float(comp);
							}
						}
					}
				}

				//ゲイン調節
				for (unsigned char ch = 0; ch < info.channel; ++ch)
				{
					data[index + ch] *= (*itr)->GetConvertParam().rate - 0.2f;
				}

				if (flag == false)
				{
					(*itr)->gap += (*itr)->GetConvertParam().gap;
				}
				else
				{
					if ((*itr)->loop == false)
					{
						break;
					}
				}
				index += info.channel;
			}

			++itr;
		}
		else
		{
			itr = okdio.erase(itr);
		}
	}
}

// 開始
void OkdioEngine::Start(void)
{
	auto hr = audio->Start();
	_ASSERT(hr == S_OK);
}

// 停止
void OkdioEngine::Stop(void)
{
	auto hr = audio->Stop();
	_ASSERT(hr == S_OK);
}
