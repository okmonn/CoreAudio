#pragma once
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class ShaderConfig
{
public:
	// コンストラクタ
	ShaderConfig(const size_t& attributeSize, const size_t& payloadSize);
	// デストラクタ
	~ShaderConfig();

	// サブオブジェクトの取得
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// サブオブジェクト
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
