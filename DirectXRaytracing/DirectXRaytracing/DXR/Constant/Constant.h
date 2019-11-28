#pragma once
#include "../Descriptor/Descriptor.h"

class Constant : 
	public Descriptor
{
public:
	// コンストラクタ
	Constant(const size_t& size, const size_t& num);
	// デストラクタ
	~Constant();

	// リソースの取得
	ID3D12Resource* Rsc(const size_t& index) const;

private:
	// リソースの生成
	void CreateRsc(const size_t& size, const size_t& num);
};
