#pragma once
#include <memory>

struct ID3D12Fence1;
class Queue;

class Fence
{
public:
	// �R���X�g���N�^
	Fence(std::weak_ptr<Queue>queue);
	// �f�X�g���N�^
	~Fence();

	// �R�}���h�ҋ@
	void Wait(void);

	// �t�F���X�̎擾
	ID3D12Fence1* Fen(void) const;

private:
	// �t�F���X�̐���
	void CreateFence(void);


	// �L���[
	std::weak_ptr<Queue>queue;

	// �t�F���X
	ID3D12Fence1* fence;

	// �t�F���X�J�E���g
	unsigned __int64 fenceCnt;

	// �t�F���X�C�x���g
	void* eventHandle;
};
