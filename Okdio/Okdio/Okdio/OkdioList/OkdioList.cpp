#include "OkdioList.h"
#include "../Okdio.h"

// コンストラクタ
OkdioList::OkdioList()
{
}

// デストラクタ
OkdioList::~OkdioList()
{
}

// リスト追加
void OkdioList::AddList(Okdio** okdio)
{
	std::unique_lock<std::mutex>lock(mtx);
	this->okdio.push_back(*okdio);
}

// リスト削除
void OkdioList::DeleteList(Okdio* okdio)
{
	std::unique_lock<std::mutex>lock(mtx);
	for (auto itr = this->okdio.begin(); itr != this->okdio.end(); ++itr)
	{
		if ((*itr) == okdio)
		{
			this->okdio.erase(itr);
			break;
		}
	}
}

// リスト取得
std::list<Okdio*> OkdioList::GetList(void) const
{
	return okdio;
}
