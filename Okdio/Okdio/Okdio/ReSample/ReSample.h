#pragma once
#include "../Base/Singleton.h"
#include "../Infomatin.h"
#include <vector>

struct IMFMediaType;
struct IMFTransform;

class ReSample : 
	public okmonn::Singleton<ReSample>
{
	friend okmonn::Singleton<ReSample>;
public:
	// 開始
	void Start(const float& quality = 1.0f);

	// リサンプル
	std::vector<float> Convert(const std::vector<float>& data, const okmonn::SoundInfo& input, const okmonn::SoundInfo& output);

	// 終了
	void Finish(void);

private:
	// コンストラクタ
	ReSample();
	// デストラクタ
	~ReSample();

	// 入力メディアの初期化
	void InitInputType(void);

	// 出力メディアの初期化
	void InitOutputType(void);

	// 入力メディアの設定
	void SetInputType(const okmonn::SoundInfo& info);

	// 出力メディアの設定
	void SetOutputType(const okmonn::SoundInfo& info);

	// 開始メッセージの送信
	void SendStartMsg(void);

	// 入力データの設定
	void SetInputData(void* desc, const std::vector<float>& input);

	// 出力データの取得
	void GetOutputData(void* desc, std::vector<float>& output);

	// 残りのデータ取得メッセージの送信
	void SendRemainingData(void);


	// 入力メディア
	IMFMediaType* inputType;

	// 出力メディア
	IMFMediaType* outputType;

	// 変換インターフェイス
	IMFTransform* transform;
};
