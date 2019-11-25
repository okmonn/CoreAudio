#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

struct D3D12_CPU_DESCRIPTOR_HANDLE;
class List;
class Swap;

class Render : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Render(std::weak_ptr<Swap>swap);
	// �f�X�g���N�^
	~Render();

	// �����_�[�^�[�Q�b�g�̃N���A
	void Clear(std::weak_ptr<List>list, float* color, const D3D12_CPU_DESCRIPTOR_HANDLE* depth = nullptr);

	// ���\�[�X�̎擾
	ID3D12Resource* Rsc(void) const;

private:
	// ������
	void Init(void);

	// �X���b�v
	std::weak_ptr<Swap>swap;
};