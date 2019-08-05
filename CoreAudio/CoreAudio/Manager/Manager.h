#pragma once
#include "../etc/Singleton.h"

struct _GUID;
typedef _GUID GUID;


class Manager : 
	public Singleton<Manager>
{
	friend Singleton<Manager>;
public:
	// オブジェクト生成
	long CreateObj(const GUID& id, void** obj);
	
private:
	// コンストラクタ
	Manager();
	// デストラクタ
	~Manager();
};
