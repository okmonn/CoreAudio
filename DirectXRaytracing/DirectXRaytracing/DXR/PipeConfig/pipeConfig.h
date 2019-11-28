#pragma once
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class PipeConfig
{
public:
	// コンストラクタ
	PipeConfig(const size_t& depth);
	// デストラクタ
	~PipeConfig();

	// サブオブジェクト
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// サブオブジェクト
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
