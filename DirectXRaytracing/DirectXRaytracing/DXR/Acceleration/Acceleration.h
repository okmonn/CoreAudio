#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

struct D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS;
struct D3D12_RAYTRACING_INSTANCE_DESC;
class List;
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
	Acceleration(std::weak_ptr<List>list, const DXR::AccelerationType& type, const size_t& instanceNum);
	// �f�X�g���N�^
	~Acceleration();

	// �g�b�v���x���̐���
	void CreateTop(const size_t& instanceNum, const size_t& rayNum, const Acceleration* bottom, const size_t& bottomNum, const Matrix3x4* initMat);

	// �{�g�����x���̐���
	void CreateBottom(ID3D12Resource* vertex);

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
	void UpData(const Matrix3x4* matrix);


	// ���X�g
	std::weak_ptr<List>list;

	// �r���h���͏��
	std::unique_ptr<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS>input;

	// �C���X�^���X��
	size_t instanceNum;

	// �o�b�t�@
	D3D12_RAYTRACING_INSTANCE_DESC* buf;
};
