#include "List.h"
#include "../Device/Device.h"
#include <d3d12.h>
#include <crtdbg.h>

// �R���X�g���N�^
List::List(const D3D12_COMMAND_LIST_TYPE& type) : 
	list(nullptr)
{
	CreateList(type);
}

// �f�X�g���N�^
List::~List()
{
	if (list != nullptr)
	{
		list->Release();
		list = nullptr;
	}
}

// �R�}���h���X�g�̐���
void List::CreateList(const D3D12_COMMAND_LIST_TYPE& type)
{
	auto hr = Device::Get().Dev()->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAGS::D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&list));
	_ASSERT(hr == S_OK);
}

// �R�}���h���X�g�̎擾
ID3D12GraphicsCommandList5* List::Lis(void) const
{
	return list;
}
