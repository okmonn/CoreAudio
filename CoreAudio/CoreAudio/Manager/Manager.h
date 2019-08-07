#pragma once
#include "../etc/Singleton.h"
#include "../etc/Info.h"

struct _GUID;
typedef _GUID GUID;


class Manager : 
	public Singleton<Manager>
{
	friend Singleton<Manager>;
public:
	// オブジェクト生成
	long CreateObj(const unsigned int& index, const okmonn::DeviceType& type, const GUID& id, void** obj);
	
private:
	// コンストラクタ
	Manager();
	// デストラクタ
	~Manager();
};
