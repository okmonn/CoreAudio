#include "Manager.h"
#include "../AudioEngin/AudioEngin.h"

// コンストラクタ
Manager::Manager()
{
}

// デストラクタ
Manager::~Manager()
{
}

// オブジェクト生成
long Manager::CreateObj(const GUID& id, void** obj)
{
	if (obj == nullptr)
	{
		return E_INVALIDARG;
	}

	*obj = nullptr;
	if (id == __uuidof(AudioEngin))
	{
		*obj = new AudioEngin();
	}
	else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}
