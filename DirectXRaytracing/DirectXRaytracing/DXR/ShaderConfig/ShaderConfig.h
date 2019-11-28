#pragma once
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class ShaderConfig
{
public:
	// �R���X�g���N�^
	ShaderConfig(const size_t& attributeSize, const size_t& payloadSize);
	// �f�X�g���N�^
	~ShaderConfig();

	// �T�u�I�u�W�F�N�g�̎擾
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// �T�u�I�u�W�F�N�g
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
