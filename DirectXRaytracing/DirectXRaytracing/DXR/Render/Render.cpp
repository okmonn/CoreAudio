#include "Render.h"
#include "../Device/Device.h"
#include "../List/List.h"
#include "../Swap/Swap.h"
#include <d3d12.h>
#include <dxgi1_6.h>

// �R���X�g���N�^
Render::Render(std::weak_ptr<Swap>swap) : swap(swap)
{
	rsc.resize(swap.lock()->Buffer());

	Init();
}

// �f�X�g���N�^
Render::~Render()
{
	for (auto& i : rsc)
	{
		if (i != nullptr)
		{
			i->Release();
			i = nullptr;
		}
	}
	if (heap != nullptr)
	{
		heap->Release();
		heap = nullptr;
	}
}

// ������
void Render::Init(void)
{
	CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rsc.size());

	for (size_t i = 0; i < rsc.size(); ++i)
	{
		auto hr = swap.lock()->Swp()->GetBuffer(i, IID_PPV_ARGS(&rsc[i]));
		_ASSERT(hr == S_OK);

		RTV(i);
	}
}

// �����_�[�^�[�Q�b�g�̃N���A
void Render::Clear(std::weak_ptr<List> list, float* color, const D3D12_CPU_DESCRIPTOR_HANDLE* depth)
{
	auto rtv = heap->GetCPUDescriptorHandleForHeapStart();
	rtv.ptr += Device::Get().Dev()->GetDescriptorHandleIncrementSize(heap->GetDesc().Type);

	list.lock()->Lis()->OMSetRenderTargets(1, &rtv, false, depth);
	list.lock()->Lis()->ClearRenderTargetView(rtv, color, 0, nullptr);
}

// ���\�[�X�̎擾
ID3D12Resource* Render::Rsc(void) const
{
	return rsc[swap.lock()->CurrentIndex()];
}
