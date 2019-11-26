#pragma once
#include "../Descriptor/Descriptor.h"

class Constant : 
	public Descriptor
{
public:
	// コンストラクタ
	Constant(const unsigned int& size, const unsigned int& num);
	// デストラクタ
	~Constant();

	// リソースの取得
	ID3D12Resource* Rsc(const unsigned int& index) const;

private:
	// リソースの生成
	void CreateRsc(const unsigned int& index, const unsigned int& size, const unsigned int& num);
};
