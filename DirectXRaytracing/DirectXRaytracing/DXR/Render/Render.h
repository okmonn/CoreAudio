#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

struct D3D12_CPU_DESCRIPTOR_HANDLE;
class List;
class Swap;

class Render : 
	public Descriptor
{
public:
	// コンストラクタ
	Render(std::weak_ptr<Swap>swap);
	// デストラクタ
	~Render();

	// リソースの取得
	ID3D12Resource* Rsc(void) const;

private:
	// 初期化
	void Init(void);

	// スワップ
	std::weak_ptr<Swap>swap;
};
