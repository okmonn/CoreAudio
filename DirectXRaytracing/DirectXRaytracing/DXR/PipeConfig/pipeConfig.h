#pragma once
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class PipeConfig
{
public:
	// �R���X�g���N�^
	PipeConfig(const size_t& depth);
	// �f�X�g���N�^
	~PipeConfig();

	// �T�u�I�u�W�F�N�g
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// �T�u�I�u�W�F�N�g
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
