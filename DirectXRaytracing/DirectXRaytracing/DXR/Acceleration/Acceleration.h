#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

struct D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS;
struct D3D12_RAYTRACING_INSTANCE_DESC;
class List;
class Primitive;
class Matrix3x4;

namespace DXR
{
	// ���z�\���̃^�C�v
	enum class AccelerationType
	{
		bottom = 2,
		top
	};
}

class Acceleration : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Acceleration(std::weak_ptr<List>list, const DXR::AccelerationType& type);
	// �f�X�g���N�^
	~Acceleration();

	// �g�b�v���x���̐���
	void CreateTop(const size_t& rayNum, Acceleration* bottom, const size_t& bottomNum);

	// �{�g�����x���̐���
	void CreateBottom(std::weak_ptr<Primitive>prim);

	// ���U���g���\�[�X�̎擾
	ID3D12Resource* Result(void) const;

	// �X�N���b�`���\�[�X�̎擾
	ID3D12Resource* Scratch(void) const;

	// �C���X�^���X���\�[�X�̎擾
	ID3D12Resource* Instance(void) const;

	// �C���X�^���X���̎擾
	size_t InstanceNum(void) const;

private:
	// �r���h
	void Build(void);

	// �g�b�v���x���̍X�V
	void UpData(void);


	// ���X�g
	std::weak_ptr<List>list;

	// �v���~�e�B�u
	std::weak_ptr<Primitive>prim;

	// �r���h���͏��
	std::unique_ptr<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS>input;

	// �C���X�^���X��
	size_t instanceNum;

	// �o�b�t�@
	D3D12_RAYTRACING_INSTANCE_DESC* buf;

	// �{�g�����x��
	std::vector<Acceleration*>bottom;
};
