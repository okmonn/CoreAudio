#pragma once
#include "../Descriptor/Descriptor.h"
#include "../Vector3.h"

class Primitive : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Primitive(const Vec3f* vertex, const size_t& vertexNum);
	// �f�X�g���N
	~Primitive();

	// ���_���\�[�X�̎擾
	ID3D12Resource* Rsc(void) const;

private:
	// ���\�[�X�̐���
	void CreateRsc(const Vec3f* vertex, const size_t& vertexNum);
};
