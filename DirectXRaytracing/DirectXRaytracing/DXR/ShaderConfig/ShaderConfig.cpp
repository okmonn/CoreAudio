#include "ShaderConfig.h"
#include <d3d12.h>

// コンストラクタ
ShaderConfig::ShaderConfig(const size_t& attributeSize, const size_t& payloadSize) : 
	sub(std::make_unique<D3D12_STATE_SUBOBJECT>())
{
	D3D12_RAYTRACING_SHADER_CONFIG config{};
	config.MaxAttributeSizeInBytes = attributeSize;
	config.MaxPayloadSizeInBytes   = payloadSize;

	sub->pDesc = &config;
	sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
}

// デストラクタ
ShaderConfig::~ShaderConfig()
{
}

// サブオブジェクトの取得
D3D12_STATE_SUBOBJECT ShaderConfig::Sub(void) const
{
	return *sub;
}
