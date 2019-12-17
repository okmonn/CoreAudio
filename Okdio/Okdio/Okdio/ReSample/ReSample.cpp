#include "ReSample.h"
#include <wrl.h>
#include <mfapi.h>
#include <mftransform.h>
#include <Mferror.h>
#include <wmcodecdsp.h>
#include <ks.h>
#include <ksmedia.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid")

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
ReSample::ReSample() : 
	inputType(nullptr), outputType(nullptr), transform(nullptr)
{
	InitInputType();
	InitOutputType();
}

// デストラクタ
ReSample::~ReSample()
{
}

// 入力メディアの初期化
void ReSample::InitInputType(void)
{
	auto hr = MFCreateMediaType(&inputType);
	_ASSERT(hr == S_OK);

	hr = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	_ASSERT(hr == S_OK);
	hr = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
	_ASSERT(hr == S_OK);
	hr = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, sizeof(float) * 8);
	_ASSERT(hr == S_OK);
	hr = inputType->SetUINT32(MF_MT_AUDIO_PREFER_WAVEFORMATEX, true);
	_ASSERT(hr == S_OK);
	hr = inputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, true);
	_ASSERT(hr == S_OK);
}

// 出力メディアの初期化
void ReSample::InitOutputType(void)
{
	auto hr = MFCreateMediaType(&outputType);
	_ASSERT(hr == S_OK);

	//入力メディアの内容コピー
	hr = inputType->CopyAllItems(outputType);
	_ASSERT(hr == S_OK);
}

// 開始
void ReSample::Start(const float& quality)
{
	auto hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IUnknown>unknown = nullptr;
	hr = CoCreateInstance(CLSID_CResamplerMediaObject, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&unknown));
	_ASSERT(hr == S_OK);

	hr = unknown->QueryInterface(IID_PPV_ARGS(&transform));
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr< IWMResamplerProps>prop = nullptr;
	hr = unknown->QueryInterface(IID_PPV_ARGS(&prop));
	_ASSERT(hr == S_OK);

	//品質を設定
	hr = prop->SetHalfFilterLength(long(60.0f * quality));
	_ASSERT(hr == S_OK);
}

// 入力メディアの設定
void ReSample::SetInputType(const okmonn::SoundInfo& info)
{
	auto hr = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, info.channel);
	_ASSERT(hr == S_OK);
	hr = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, info.sample);
	_ASSERT(hr == S_OK);
	hr = inputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, sizeof(float) * info.channel);
	_ASSERT(hr == S_OK);
	hr = inputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, info.sample * info.channel * sizeof(float));
	_ASSERT(hr == S_OK);
	hr = transform->SetInputType(0, inputType, 0);
	_ASSERT(hr == S_OK);
}

// 出力メディアの設定
void ReSample::SetOutputType(const okmonn::SoundInfo& info)
{
	auto hr = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, info.channel);
	_ASSERT(hr == S_OK);
	hr = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, info.sample);
	_ASSERT(hr == S_OK);
	hr = outputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, sizeof(float) * info.channel);
	_ASSERT(hr == S_OK);
	hr = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, info.sample * info.channel * sizeof(float));
	_ASSERT(hr == S_OK);
	hr = transform->SetOutputType(0, outputType, 0);
	_ASSERT(hr == S_OK);
}

// 開始メッセージの送信
void ReSample::SendStartMsg(void)
{
	auto hr = transform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
	_ASSERT(hr == S_OK);
	hr = transform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
	_ASSERT(hr == S_OK);
	hr = transform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);
	_ASSERT(hr == S_OK);

}

// 入力データの設定
void ReSample::SetInputData(void* desc, const std::vector<float>& input)
{
	Microsoft::WRL::ComPtr<IMFMediaBuffer>buf = nullptr;
	auto hr = MFCreateMemoryBuffer(sizeof(float) * input.size(), &buf);
	_ASSERT(hr == S_OK);

	//データのコピー
	unsigned char* tmp = nullptr;
	hr = buf->Lock(&tmp, nullptr, nullptr);
	_ASSERT(hr == S_OK);
	std::memcpy(tmp, input.data(), sizeof(float) * input.size());
	hr = buf->Unlock();
	_ASSERT(hr == S_OK);

	//有効範囲の設定
	hr = buf->SetCurrentLength(sizeof(float) * input.size());
	_ASSERT(hr == S_OK);

	//バッファの追加
	hr = ((MFT_OUTPUT_DATA_BUFFER*)desc)->pSample->AddBuffer(buf.Get());
	_ASSERT(hr == S_OK);

	//サンプラーのセット
	hr = transform->ProcessInput(0, ((MFT_OUTPUT_DATA_BUFFER*)desc)->pSample, 0);
	_ASSERT(hr == S_OK);
}

// 出力データの取得
void ReSample::GetOutputData(void* desc, std::vector<float>& output)
{
	std::vector<float>data;
	while (true)
	{
		auto hr = transform->ProcessOutput(0, 1, (MFT_OUTPUT_DATA_BUFFER*)desc, &((MFT_OUTPUT_DATA_BUFFER*)desc)->dwStatus);
		if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT)
		{
			break;
		}

		Microsoft::WRL::ComPtr<IMFMediaBuffer>buf = nullptr;
		hr = ((MFT_OUTPUT_DATA_BUFFER*)desc)->pSample->ConvertToContiguousBuffer(&buf);
		_ASSERT(hr == S_OK);

		//有効範囲の取得
		unsigned long size = 0;
		hr = buf->GetCurrentLength(&size);
		_ASSERT(hr == S_OK);

		//配列のサイズ設定
		data.resize(size / sizeof(float));

		//データのコピー
		unsigned char* tmp = nullptr;
		hr = buf->Lock(&tmp, nullptr, nullptr);
		_ASSERT(hr == S_OK);
		std::memcpy(data.data(), tmp, size);
		hr = buf->Unlock();
		_ASSERT(hr == S_OK);

		//出力データに追加
		output.insert(output.end(), data.begin(), data.end());
	}
}

// 残りのデータ取得メッセージの送信
void ReSample::SendRemainingData(void)
{
	auto hr = transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
	_ASSERT(hr == S_OK);
	hr = transform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
	_ASSERT(hr == S_OK);
}

// リサンプル
std::vector<float> ReSample::Convert(const std::vector<float>& data, const okmonn::SoundInfo& input, const okmonn::SoundInfo& output)
{
	SetInputType(input);
	SetOutputType(output);
	SendStartMsg();

	MFT_OUTPUT_DATA_BUFFER desc{};
	auto hr = MFCreateSample(&desc.pSample);
	_ASSERT(hr == S_OK);

	SetInputData((void*)&desc, data);

	std::vector<float>tmp;
	GetOutputData((void*)&desc, tmp);
	SendRemainingData();
	GetOutputData((void*)&desc, tmp);

	return tmp;
}

// 終了
void ReSample::Finish(void)
{
	auto hr = transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, 0);
	_ASSERT(hr == S_OK);

	if (outputType != nullptr)
	{
		outputType->Release();
		outputType = nullptr;
	}
	if (inputType != nullptr)
	{
		inputType->Release();
		inputType = nullptr;
	}
	if (transform != nullptr)
	{
		transform->Release();
		transform = nullptr;
	}

	hr = MFShutdown();
	_ASSERT(hr == S_OK);
}
