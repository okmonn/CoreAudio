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
		hr = enumerator->EnumAudioEndpoints(EDataFlow(devType), DEVICE_STATE_ACTIVE, &collection);
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
		hr = enumerator->GetDefaultAudioEndpoint(EDataFlow(devType), ERole::eConsole, &device);
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
			hr = audio->GetMixFormat((WAVEFORMATEX**)&fmt);
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
	auto hr = device->Activate(__uuidof(audio), CLSCTX_INPROC_SERVER, nullptr, (void**)&audio);
	_ASSERT(hr == S_OK);

	//�t�H�[�}�b�g�̃`�F�b�N
	WAVEFORMATEXTENSIBLE* fmt   = (WAVEFORMATEXTENSIBLE*)SetFormat(audio.Get());
	WAVEFORMATEXTENSIBLE* corre = nullptr;
	{
		hr = audio->IsFormatSupported(AUDCLNT_SHAREMODE(audioType), (WAVEFORMATEX*)fmt, (WAVEFORMATEX**)&corre);
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
		defPeriod, defPeriod * int(audioType), (WAVEFORMATEX*)&fmt, nullptr);
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
			defPeriod, defPeriod * int(audioType), (WAVEFORMATEX*)&fmt, nullptr);
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
			if (index + ch < convert.size())
			{
				convert[index + ch] *= param.rate - 0.2f;
			}
		}
		index += info.channel;
		gap += param.gap;
	}

	return convert;
}

void BPM(const std::vector<float>& data, const okmonn::AudioInfo& info)
{
	//1�t���[���̃T���v����
	unsigned int sample = 512 * info.channel;
	//�t���[����
	unsigned int fream = data.size() / sample;
	//1�t���[���̃T���v�����O���g��
	double freamSample = double(info.sample) / double(sample);

	//�t���[�����Ƃ̉���
	std::vector<double>vol(fream);
	{
		for (unsigned int i = 0; i < fream; ++i)
		{
			double tmp = 0;
			for (unsigned int n = 0; n < sample; ++n)
			{
				tmp += std::pow(data[i * sample + n], 2);
			}
			vol[i] = std::sqrt(tmp / sample);
		}
	}

	//�ׂ荇���t���[���̑����ʂ����߂�
	std::vector<double>incre(fream);
	{
		for (unsigned int i = 1; i < fream; ++i)
		{
			incre[i - 1] = vol[i] - vol[i - 1];
			if (incre[i - 1] < 0.0)
			{
				incre[i - 1] = 0.0;
			}
		}
	}

	//�����ʂ̎��ԕω��̎��g�����������߂�
	std::vector<std::vector<double>>bpm(3, std::vector<double>(240 - 60 + 1, 0));
	{
		for (size_t i = 0; i < bpm[0].size(); ++i)
		{
			double fbpm = double(60LL + i) / 60.0;
			for (unsigned int n = 0; n < fream; ++n)
			{
				double winFunc = okmonn::Hanning<float>(n, fream);
				bpm[0][i] += incre[n] * std::cos(2.0 * std::acos(-1.0) * fbpm * n / freamSample) * winFunc;
				bpm[1][i] += incre[n] * std::sin(2.0 * std::acos(-1.0) * fbpm * n / freamSample) * winFunc;
			}
			bpm[0][i] /= sample;
			bpm[1][i] /= sample;
			bpm[2][i] = std::sqrt(std::pow(bpm[0][i], 2) + std::pow(bpm[1][i], 2));
		}
	}

	//�s�[�N���o
	std::vector<int>peak(3, -1);
	{
		for (unsigned int i = 1; i < bpm[0].size(); ++i)
		{
			for (size_t n = 0; n < peak.size(); ++n)
			{
				if (peak[n] < 0 || bpm[0][i - 1] > bpm[0][peak[n]])
				{
					for (size_t m = n + 1; m < peak.size(); ++m)
					{
						std::swap(peak[m - 1], peak[m]);
					}
					peak[n] = i - 1;
					break;
				}
			}
		}
	}

	double start_n;
	double beat_gap;
	for (int idx = 0; idx < 3; idx++)
	{
		if (peak[idx] < 0)
		{
			break;
		}

		printf("[%d]\n", idx + 1);

		int peak_bpm = peak[idx] + 60;
		printf("peak bmp : %d\n", peak_bpm);

		// �ʑ���
		double theta = atan2(bpm[1][peak[idx]], bpm[0][peak[idx]]);
		if (theta < 0)
		{
			theta += 2.0 * std::acos(-1.0);
		}
		double peak_f = double(peak_bpm) / 60;
		double start_time = theta / (2.0 * std::acos(-1.0) * peak_f);
		double start_beat = theta / (2.0 * std::acos(-1.0));
		printf("first beat time : %f sec\n", start_time);
		printf("first beat : %f beat\n", start_beat);

		double ajust_beat = (2.0 * std::acos(-1.0) - theta) / (2.0 * std::acos(-1.0));
		int ajust_beat1 = int(ajust_beat * 4) % 4;
		int ajust_beat2 = int(ajust_beat * 4 * 120) % 120;
		printf("ajust beat for cubase : 1 . 1 . %d . %d\n", ajust_beat1 + 1, ajust_beat2);

		// ���ʒu
		if (idx == 0)
		{
			start_n = int(start_time * freamSample);
			beat_gap = freamSample / peak_f;
		}
	}
}

std::vector<float> PSOLA(const std::vector<float>& data, const okmonn::AudioInfo& info, const float& rate)
{
	//���֊֐��͈�
	const unsigned int size = (info.sample * 0.01) * info.channel;
	//����ŏ��l
	const unsigned int min  = size / 2;
	//����ő�l
	const unsigned int max  = size * 2;

	std::vector<float>convert(data.size() * 2);

	unsigned int offset0 = 0;
	unsigned int offset1 = 0;

	while (offset0 + max < data.size())
	{
		//���ȑ���
		float peak = 0.0;
		int index = min;
		for (unsigned int i = min; i <= max; ++i)
		{
			float tmp = 0.0f;
			for (unsigned int n = 0; n < size; ++n)
			{
				tmp += data[n] * data[i + n];
			}

			//�s�[�N�l�X�V
			if (peak < tmp)
			{
				peak  = tmp;
				index = i;
			}
		}

		int tmp = index * rate;
		std::vector<float>cut(&data[std::fmax(offset0 + index - tmp, 0)], &data[offset0 + index + tmp]);
		for (size_t i = 0; i < cut.size(); ++i)
		{
			cut[i] *= okmonn::Hanning<float>(i, cut.size());
		}

		for (size_t i = 0; i < cut.size(); ++i)
		{
			convert[offset1 + i] += cut[i];
		}

		offset0 += index;
		offset1 += cut.size() / 2;
	}

	return convert;
}

// �񓯊�����
void OkdioEngine::Stream(void)
{
	std::string name = "SOS.wav";
	auto q = SoundLoader::Get().Load(name);
	auto wave1 = *SoundLoader::Get().GetWave(name);
	auto waveInfo = SoundLoader::Get().GetInfo(name);
	BPM(wave1, waveInfo);
	auto wave2 = PSOLA(wave1, waveInfo, 2.0f);
	waveInfo.sample = waveInfo.sample * 2.0f;
	auto param = okmonn::GetConvertParam(waveInfo.sample, info.sample);
	wave2 = Resampling(*SoundLoader::Get().GetConvertCorre(name), param, wave2, waveInfo);
	std::vector<int>in(wave2.size());
	for (size_t i = 0; i < in.size(); ++i)
	{
		in[i] = wave2[i] * (0xffff / 2) * 0xffff;
	}
	unsigned int index = 0;
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
			size = wave2.size()  - index;
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
