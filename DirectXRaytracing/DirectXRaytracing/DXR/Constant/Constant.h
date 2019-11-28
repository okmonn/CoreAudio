#pragma once
#include "../Descriptor/Descriptor.h"

class Constant : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Constant(const size_t& size, const size_t& num);
	// �f�X�g���N�^
	~Constant();

	// ���\�[�X�̎擾
	ID3D12Resource* Rsc(const size_t& index) const;

private:
	// ���\�[�X�̐���
	void CreateRsc(const size_t& size, const size_t& num);
};
