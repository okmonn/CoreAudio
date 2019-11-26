#include "Render.h"
#include "../Device/Device.h"
#include "../List/List.h"
#include "../Swap/Swap.h"
#include <d3d12.h>
#include <dxgi1_6.h>

// コンストラクタ
Render::Render(std::weak_ptr<Swap>swap) : swap(swap)
{
	rsc.resize(swap.lock()->Buffer());

	Init();
}

// デストラクタ
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

// 初期化
void Render::Init(void)
{
	CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (size_t i = 0; i < rsc.size(); ++i)
	{
		auto hr = swap.lock()->Swp()->GetBuffer(unsigned int(i), IID_PPV_ARGS(&rsc[i]));
		_ASSERT(hr == S_OK);

		RTV(i);
	}
}

// リソースの取得
ID3D12Resource* Render::Rsc(void) const
{
	return rsc[swap.lock()->CurrentIndex()];
}
