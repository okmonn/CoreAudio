#include "pipeConfig.h"
#include <d3d12.h>

// �R���X�g���N�^
PipeConfig::PipeConfig(const size_t& depth) : 
	sub(std::make_unique<D3D12_STATE_SUBOBJECT>())
{
	D3D12_RAYTRACING_PIPELINE_CONFIG config{};
	config.MaxTraceRecursionDepth = depth;

	sub->pDesc = &config;
	sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
}

// �f�X�g���N�^
PipeConfig::~PipeConfig()
{
}

// �T�u�I�u�W�F�N�g
D3D12_STATE_SUBOBJECT PipeConfig::Sub(void) const
{
	return *sub;
}
