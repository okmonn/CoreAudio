#include "OkdioList.h"
#include "../Okdio.h"

// �R���X�g���N�^
OkdioList::OkdioList()
{
}

// �f�X�g���N�^
OkdioList::~OkdioList()
{
}

// ���X�g�ǉ�
void OkdioList::AddList(Okdio** okdio)
{
	std::unique_lock<std::mutex>lock(mtx);
	this->okdio.push_back(*okdio);
}

// ���X�g�폜
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

// ���X�g�擾
std::list<Okdio*> OkdioList::GetList(void) const
{
	return okdio;
}