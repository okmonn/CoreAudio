#pragma once
#include "../Base/Singleton.h"
#include <list>
#include <mutex>

class Okdio;

class OkdioList :
	public okmonn::Singleton<OkdioList>
{
	friend okmonn::Singleton<OkdioList>;

public:
	// リスト追加
	void AddList(Okdio** okdio);

	// リスト削除
	void DeleteList(Okdio* okdio);

	// リスト取得
	std::list<Okdio*> GetList(void) const;

private:
	// コンストラクタ
	OkdioList();
	// デストラクタ
	~OkdioList();


	// 排他制御
	std::mutex mtx;

	// Okdioリスト
	std::list<Okdio*>okdio;
};
