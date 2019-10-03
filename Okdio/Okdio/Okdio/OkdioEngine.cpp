#include "Okdio.h"
#include "SoundLoader/SoundLoader.h"
#include <ks.h>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

// �X�s�[�J�ꗗ
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
		CloseHandle(i);
	}
}

// Okdio����
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

// �I�[�f�B�I�f�o�C�X����
void OkdioEngine::CreateDevice(const char& deviceIndex)
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
		hr = enumerator->EnumAudioEndpoints(EDataFlow(audioType), DEVICE_STATE_ACTIVE, &collection);
		_ASSERT(hr == S_OK);
	}

	//�R���N�V�������擾
	unsigned int deviceNum = 0;
	{
		hr = collection->GetCount(&deviceNum);
		_ASSERT(hr == S_OK);
	}

	//�f�t�H���g�f�o�C�X�g�p
	if (deviceIndex < 0 || deviceNum <= unsigned int(deviceIndex))
	{
		hr = enumerator->GetDefaultAudioEndpoint(EDataFlow(audioType), ERole::eConsole, &device);
		_ASSERT(hr == S_OK);
	}
	//�w��f�o�C�X�g�p
	else
	{
		hr = collection->Item(deviceIndex, &device);
		_ASSERT(hr == S_OK);
	}
}

// �t�H�[�}�b�g�ݒ�
void* OkdioEngine::SetFormat(IAudioClient* audio, const okmonn::AudioInfo* info)
{
	long hr = S_OK;

	WAVEFORMATEXTENSIBLE* fmt = nullptr;
	//�f�o�C�X�̃t�H�[�}�b�g��ݒ�
	if (info == nullptr)
	{
		//���L���[�h
		if (audioType == okmonn::AudioType::SHARED)
		{
			hr = audio->GetMixFormat((WAVEFORMATEX * *)& fmt);
			_ASSERT(hr == S_OK);
		}
		//�r�����[�h
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
	//�w�肵���t�H�[�}�b�g��ݒ�
	else
	{
		fmt = new WAVEFORMATEXTENSIBLE();

		fmt->Format.cbSize          = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		fmt->Format.nSamplesPerSec  = info->sample;
		fmt->Format.wBitsPerSample  = info->byte * 8;
		fmt->Format.nChannels       = info->channel;
		fmt->Format.nBlockAlign     = fmt->Format.nChannels * fmt->Format.wBitsPerSample / 8;
		fmt->Format.nAvgBytesPerSec = fmt->Format.nSamplesPerSec * fmt->Format.nBlockAlign;
		fmt->Format.wFormatTag      = WAVE_FORMAT_EXTENSIBLE;

		fmt->dwChannelMask               = spk[fmt->Format.nChannels - 1];
		fmt->Samples.wValidBitsPerSample = fmt->Format.wBitsPerSample;
		fmt->SubFormat                   = (info->flag == 0) ? KSDATAFORMAT_SUBTYPE_PCM : KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	return fmt;
}

// �I�[�f�B�I�N���C�A���g����
void OkdioEngine::CreateAudioClient(void)
{
	//�I�[�f�B�I�N���C�A���g����
	auto hr = device->Activate(__uuidof(audio), CLSCTX_INPROC_SERVER, nullptr, (void**)& audio);
	_ASSERT(hr == S_OK);

	//�t�H�[�}�b�g�̃`�F�b�N
	WAVEFORMATEXTENSIBLE* fmt   = (WAVEFORMATEXTENSIBLE*)SetFormat(audio.Get());
	WAVEFORMATEXTENSIBLE* corre = nullptr;
	{
		hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)fmt, (WAVEFORMATEX * *)& corre);
		if (hr == S_OK)
		{
			info.sample  = fmt->Format.nSamplesPerSec;
			info.byte    = unsigned char(fmt->Format.wBitsPerSample / 8);
			info.channel = unsigned char(fmt->Format.nChannels);
			info.flag    = (fmt->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) ? 0 : 1;
		}
		else
		{
			OutputDebugStringA("\n�t�H�[�}�b�g���T�|�[�g����Ă��܂���\n�œK�ȃt�H�[�}�b�g�ɏC�����܂�\n");

			info.sample  = corre->Format.nSamplesPerSec;
			info.byte    = unsigned char(corre->Format.wBitsPerSample / 8);
			info.channel = unsigned char(corre->Format.nChannels);
			info.flag    = (corre->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) ? 0 : 1;

			hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)corre, nullptr);
			_ASSERT(hr == S_OK);
		}
	}
}

// �I�[�f�B�I�N���C�A���g������
void OkdioEngine::Initialize(void)
{
	long hr = S_OK;

	//���C�e���V�擾
	REFERENCE_TIME defPeriod = 0;
	REFERENCE_TIME minPeriod = 0;
	{
		hr = audio->GetDevicePeriod(&defPeriod, &minPeriod);
		_ASSERT(hr == S_OK);
	}

	WAVEFORMATEXTENSIBLE fmt{};
	fmt.Format.cbSize          = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	fmt.Format.nSamplesPerSec  = info.sample;
	fmt.Format.wBitsPerSample  = info.byte * 8;
	fmt.Format.nChannels       = info.channel;
	fmt.Format.nBlockAlign     = fmt.Format.nChannels * fmt.Format.wBitsPerSample / 8;
	fmt.Format.nAvgBytesPerSec = fmt.Format.nSamplesPerSec * fmt.Format.nBlockAlign;
	fmt.Format.wFormatTag      = WAVE_FORMAT_EXTENSIBLE;

	fmt.dwChannelMask               = spk[fmt.Format.nChannels - 1];
	fmt.Samples.wValidBitsPerSample = fmt.Format.wBitsPerSample;
	fmt.SubFormat                   = (info.flag == 0) ? KSDATAFORMAT_SUBTYPE_PCM : KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

	//������
	hr = audio->Initialize(AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
		defPeriod, defPeriod * int(audioType), (WAVEFORMATEX*)& fmt, nullptr);
	//�A���C�����g����Ă��Ȃ��ꍇ
	if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
	{
		//�C����̃t���[�������擾
		unsigned __int32 fream = 0;
		hr = audio->GetBufferSize(&fream);
		_ASSERT(hr == S_OK);
		defPeriod = REFERENCE_TIME(std::round(10000.0 * 1000 * fream / fmt.Format.nSamplesPerSec));

		//�Ē���
		hr = audio->Initialize(_AUDCLNT_SHAREMODE(audioType), AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
			defPeriod, defPeriod * int(audioType), (WAVEFORMATEX*)& fmt, nullptr);
	}
	_ASSERT(hr == S_OK);

	//�C�x���g�Z�b�g
	hr = audio->SetEventHandle(handle[0]);
	_ASSERT(hr == S_OK);
}

// �I�[�f�B�I�����_���[����
void OkdioEngine::CreateAudioRender(void)
{
	auto hr = audio->GetService(IID_PPV_ARGS(&render));
	_ASSERT(hr == S_OK);

	//�����o�b�t�@�N���A
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

#include "SoundLoader/SoundLoader.h"

std::vector<float> Test(const std::vector<float>& data, const okmonn::AudioInfo& info)
{
	auto fft = okmonn::FFT(data);

	return okmonn::IFFT(fft, data.size());
}

std::vector<float> Resampling(const std::vector<double>& corre, const okmonn::ConvertParam& param, const std::vector<float>& data, const okmonn::AudioInfo& info)
{
	std::vector<float>convert(data.size() * (double(param.sample) / double(info.sample)), 0);
	unsigned int offset = (corre.size() - 1) / 2;
	unsigned int index = 0;
	double gap = param.gap;
	unsigned int a = 0;
	while (index < convert.size())
	{
		double integer = 0.0;
		gap = std::modf(gap, &integer);
		offset += int(integer);
		//�ϊ�
		for (size_t i = 0; i < (corre.size() - 1); ++i)
		{
			long tmp = offset - ((corre.size() - 1) / 2) + i;
			if (size_t(tmp / param.rate) * info.channel >= data.size())
			{
				break;
			}

			if (tmp % param.rate == 0)
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
					if ((tmp / param.rate) * info.channel + ch < data.size())
					{
						convert[index + ch] += data[(tmp / param.rate) * info.channel + ch] * float(comp);
					}
				}
			}
		}

		//�Q�C������
		for (unsigned char ch = 0; ch < info.channel; ++ch)
		{
			convert[index + ch] *= param.rate - 0.2f;
		}
		index += info.channel;
		gap += param.gap;
	}

	return convert;
}

// �񓯊�����
void OkdioEngine::Stream(void)
{
	std::string name = "SOS.wav";
	auto q = SoundLoader::Get().Load(name);
	auto wave1 = SoundLoader::Get().GetWave(name);
	auto waveInfo = SoundLoader::Get().GetInfo(name);
	std::vector<float>wave2(wave1->size() / (waveInfo.byte));
	short* ptr = (short*)wave1->data();
	for (size_t i = 0; i < wave2.size(); ++i)
	{
		wave2[i] = okmonn::Normalize<float>(ptr[i]);
	}
	unsigned int index = 0;
	auto param = okmonn::GetConvertParam(waveInfo.sample, info.sample);
	wave2 = Resampling(*SoundLoader::Get().GetConvertCorre(name), param, wave2, waveInfo);

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


		//�f�[�^�̃R�s�[
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
}

// �J�n
void OkdioEngine::Start(void)
{
	auto hr = audio->Start();
	_ASSERT(hr == S_OK);
}

// ��~
void OkdioEngine::Stop(void)
{
	auto hr = audio->Stop();
	_ASSERT(hr == S_OK);
}
