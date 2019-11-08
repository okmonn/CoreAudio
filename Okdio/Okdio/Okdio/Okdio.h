#pragma once
#include "OkdioEngine.h"

class __declspec(uuid("00000000-0917-0917-0917-000000000001"))
	Okdio :
	public okmonn::Unknown<Okdio>
{
	friend OkdioEngine;

public:
	// コンストラクタ
	Okdio(const std::string& fileName);
	// デストラクタ
	~Okdio();

	// 再生
	void Play(const bool& loop = false);

	// 停止
	void Stop(void);

	// サウンド情報取得
	okmonn::SoundInfo GetInfo(void) const;

	// サウンド情報セット
	void SetInfo(const okmonn::SoundInfo& info);

	// 最後まで再生したか確認
	bool IsFinish(void) const;

private:
	// 読み込み
	void Load(const std::string& fileName);

	// 波形データをサウンドキューに追加
	void Submit(const size_t& num);

	Okdio* ptr(void)
	{
		return this;
	}


	// 参照ファイル名
	std::string name;

	// サウンド情報
	okmonn::SoundInfo info;

	// 読み込み位置
	size_t read;

	// 再生フラグ
	bool play;

	// ループフラグ
	bool loop;

	// 終了フラグ
	bool finish;
};
