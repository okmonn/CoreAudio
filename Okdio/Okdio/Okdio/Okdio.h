#pragma once
#include "OkdioEngine.h"

class __declspec(uuid("00000000-0917-0917-0917-000000000001"))
	Okdio : public UnKnown<Okdio>
{
	friend OkdioEngine;
public:
	// サウンド参照ファイル名取得
	std::string GetName(void) const;

	// サウンド情報取得
	okmonn::AudioInfo GetInfo(void) const;

	// サウンドデータ取得
	std::weak_ptr<std::vector<float>> GetWave(void) const;

	// サウンドデータ数取得
	size_t GetWaveNum(void) const;

	// サンプリング周波数変換パラメータ取得
	okmonn::ConvertParam GetConvertParam(void) const;

	// サンプリング周波数変換用係数取得
	std::weak_ptr<std::vector<double>> GetConvertCorre(void) const;

private:
	void operator=(const Okdio&) = delete;

	// コンストラクタ
	Okdio(const std::string& fileName);
	// デストラクタ
	~Okdio();

	// 参照サウンドファイル名
	std::string name;

	// インデックス
	unsigned int index;

	// サンプリング周波数変換用オフセット
	unsigned int offset;

	// サンプリング周波数変換用ずれ
	double gap;

	// 再生フラグ
	bool play;

	// ループフラグ
	bool loop;
};

