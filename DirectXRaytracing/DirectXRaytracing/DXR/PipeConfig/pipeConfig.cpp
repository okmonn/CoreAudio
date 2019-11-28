#include "pipeConfig.h"
#include <d3d12.h>

// コンストラクタ
PipeConfig::PipeConfig(const size_t& depth) : 
	sub(std::make_unique<D3D12_STATE_SUBOBJECT>())
{
	D3D12_RAYTRACING_PIPELINE_CONFIG config{};
	config.MaxTraceRecursionDepth = depth;

	sub->pDesc = &config;
	sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
}

// デストラクタ
PipeConfig::~PipeConfig()
{
}

// サブオブジェクト
D3D12_STATE_SUBOBJECT PipeConfig::Sub(void) const
{
	return *sub;
}
