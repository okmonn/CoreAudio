#pragma once
#include "../Descriptor/Descriptor.h"
#include "../Matrix.h"
#include <vector>
#include <memory>

class Constant;

class Primitive : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Primitive(const Vec3f* vertex, const size_t& vertexNum, const size_t& constantSize, const size_t& instanceNum);
	// �f�X�g���N
	~Primitive();

	// �萔�̃Z�b�g
	void SetConstant(const size_t& instanceID, void* data);

	// ���s�ړ�
	void Translation(const size_t& instanceID, const Vec3f& vec);

	// �C���X�^���X���̎擾
	size_t InstanceNum(void) const;

	// ���_���\�[�X�̎擾
	ID3D12Resource* Vertex(void) const;

	// �萔���\�[�X�̎擾
	ID3D12Resource* Const(const size_t& instanceID) const;

	// �s��̎擾
	Matrix3x4 Matrix(const size_t& instanceID);

private:
	// ���\�[�X�̐���
	void CreateRsc(const Vec3f* vertex, const size_t& vertexNum);


	// �R���X�^���g
	std::unique_ptr<Constant>constant;

	// �C���X�^���X��
	size_t instanceNum;

	// �萔�o�b�t�@
	std::vector<void*>buf;

	// �s��
	std::vector<Matrix3x4>matrix;
};
