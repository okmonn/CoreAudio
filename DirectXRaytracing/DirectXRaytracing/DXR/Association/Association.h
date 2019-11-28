#pragma once
#include <string>
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class Association
{
public:
	// �R���X�g���N�^
	Association(const std::initializer_list<std::string>& func, D3D12_STATE_SUBOBJECT* sub);
	// �f�X�g���N�^
	~Association();

	// �T�u�I�u�W�F�N�g�̎擾
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// �T�u�I�u�W�F�N�g
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
