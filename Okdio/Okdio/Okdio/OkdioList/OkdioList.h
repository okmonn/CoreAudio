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
	// ���X�g�ǉ�
	void AddList(Okdio** okdio);

	// ���X�g�폜
	void DeleteList(Okdio* okdio);

	// ���X�g�擾
	std::list<Okdio*> GetList(void) const;

private:
	// �R���X�g���N�^
	OkdioList();
	// �f�X�g���N�^
	~OkdioList();


	// �r������
	std::mutex mtx;

	// Okdio���X�g
	std::list<Okdio*>okdio;
};
