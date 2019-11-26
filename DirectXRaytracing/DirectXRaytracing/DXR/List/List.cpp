#include "List.h"
#include "../Device/Device.h"
#include "../Allocator/Allocator.h"
#include <d3d12.h>
#include <crtdbg.h>

// コンストラクタ
List::List(const D3D12_COMMAND_LIST_TYPE& type) : 
	list(nullptr)
{
	CreateList(type);
}

// デストラクタ
List::~List()
{
	if (list != nullptr)
	{
		list->Release();
		list = nullptr;
	}
}

// コマンドリストのリセット
void List::Reset(std::weak_ptr<Allocator>allo, ID3D12PipelineState* pipe)
{
	auto hr = list->Reset(allo.lock()->Allo(), pipe);
	_ASSERT(hr == S_OK);
}

// コマンドリストを閉じる
void List::Close(void)
{
	auto hr = list->Close();
	_ASSERT(hr == S_OK);
}

// コマンドリストの生成
void List::CreateList(const D3D12_COMMAND_LIST_TYPE& type)
{
	auto hr = Device::Get().Dev()->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAGS::D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&list));
	_ASSERT(hr == S_OK);
}

// 仮想構造のビルド
void List::AccelerationBuild(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& buildDesc)
{
	list->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
}

// UAVバリア
void List::Barrier(ID3D12Resource* rsc)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = rsc;
	list->ResourceBarrier(1, &barrier);
}

// コマンドリストの取得
ID3D12GraphicsCommandList5* List::Lis(void) const
{
	return list;
}
