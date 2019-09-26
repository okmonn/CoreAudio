#pragma once
#include "../ClassBase/Singleton.h"
#include "../Information.h"
#include <vector>
#include <memory>
#include <unordered_map>

class SoundLoader :
	public Singleton<SoundLoader>
{
	struct Sound {
		//サウンド情報
		okmonn::AudioInfo info;
		//サウンドデータ
		std::shared_ptr<std::vector<unsigned char>>wave;
		//サンプリング周波数変換パラメータ
		okmonn::ConvertParam param;
		//サンプリング周波数変換用係数
		std::shared_ptr<std::vector<double>>corre;
	};

	friend Singleton<SoundLoader>;
public:
	// 出力サウンド情報セット
	void SetOutInfo(const okmonn::AudioInfo& info);

	// 読み込み
	int Load(const std::string& fileName);

	// 指定データを削除
	void Delete(const std::string& fileName);

	// サウンド情報取得
	okmonn::AudioInfo GetInfo(const std::string& fileName);

	// サウンドデータ取得
	std::shared_ptr<std::vector<unsigned char>> GetWave(const std::string& fileName);

	// サウンドデータ数取得
	size_t GetWaveNum(const std::string& fileName);

	// サンプリング周波数変換パラメータ取得
	okmonn::ConvertParam GetConvertParam(const std::string& fileName);

	// サンプリング周波数変換用係数取得
	std::shared_ptr<std::vector<double>> GetConvertCorre(const std::string& fileName);

private:
	// コンストラクタ
	SoundLoader();
	// デストラクタ
	~SoundLoader();


	// 出力サウンド情報
	okmonn::AudioInfo info;

	// 読み込みデータ
	std::unordered_map<std::string, Sound>sound;
};
