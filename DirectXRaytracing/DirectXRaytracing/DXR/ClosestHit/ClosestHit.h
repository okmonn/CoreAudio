#pragma once
#include <string>
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class ClosestHit
{
public:
	// �R���X�g���N�^
	ClosestHit(const std::string& anyHit, const std::string& closestHit, const std::string& hitGroup);
	// �f�X�g���N
	~ClosestHit();

	// �T�u�I�u�W�F�N�g�̎擾
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// �T�u�I�u�W�F�N�g
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
