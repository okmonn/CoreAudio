#pragma once
#include "../Base/Singleton.h"
#include "../Infomatin.h"
#include <vector>
#include <memory>
#include <unordered_map>

class Loader : 
	public okmonn::Singleton<Loader>
{
	friend okmonn::Singleton<Loader>;

	struct Sound {
		//サウンド情報
		okmonn::SoundInfo info;
		std::shared_ptr<std::vector<float>>wave;
	};

public:
	// 読み込み
	bool Load(const std::string& fileName);

	// 削除
	bool Delete(const std::string& fileName);

	// サウンド情報取得
	okmonn::SoundInfo GetInfo(const std::string& fileName);

	// サウンドデータ取得
	std::shared_ptr<std::vector<float>> GetWave(const std::string& fileName);

	// サウンドデータ数取得
	size_t GetWaveNum(const std::string& fileName);

	// 1回の処理データ量
	size_t ProcessingNum(const std::string& fileName);

private:
	// コンストラクタ
	Loader();
	// デストラクタ
	~Loader();


	// 読み込みデータ
	std::unordered_map<std::string, Sound>sound;
};
