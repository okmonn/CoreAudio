#include "List.h"
#include "../Device/Device.h"
#include "../Allocator/Allocator.h"
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

// �R�}���h���X�g�̃��Z�b�g
void List::Reset(std::weak_ptr<Allocator>allo, ID3D12PipelineState* pipe)
{
	auto hr = list->Reset(allo.lock()->Allo(), pipe);
	_ASSERT(hr == S_OK);
}

// �R�}���h���X�g�����
void List::Close(void)
{
	auto hr = list->Close();
	_ASSERT(hr == S_OK);
}

// �R�}���h���X�g�̐���
void List::CreateList(const D3D12_COMMAND_LIST_TYPE& type)
{
	auto hr = Device::Get().Dev()->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAGS::D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&list));
	_ASSERT(hr == S_OK);
}

// ���z�\���̃r���h
void List::AccelerationBuild(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& buildDesc)
{
	list->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
}

// UAV�o���A
void List::Barrier(ID3D12Resource* rsc)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = rsc;
	list->ResourceBarrier(1, &barrier);
}

// �R�}���h���X�g�̎擾
ID3D12GraphicsCommandList5* List::Lis(void) const
{
	return list;
}
