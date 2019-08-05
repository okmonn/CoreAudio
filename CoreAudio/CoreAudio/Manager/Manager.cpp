#include "Manager.h"
#include "../AudioEngin/AudioEngin.h"

// �R���X�g���N�^
Manager::Manager()
{
}

// �f�X�g���N�^
Manager::~Manager()
{
}

// �I�u�W�F�N�g����
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
