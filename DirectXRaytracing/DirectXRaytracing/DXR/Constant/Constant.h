#pragma once
#include "../Descriptor/Descriptor.h"

class Constant : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Constant(const unsigned int& size, const unsigned int& num);
	// �f�X�g���N�^
	~Constant();

	// ���\�[�X�̎擾
	ID3D12Resource* Rsc(const unsigned int& index) const;

private:
	// ���\�[�X�̐���
	void CreateRsc(const unsigned int& index, const unsigned int& size, const unsigned int& num);
};
