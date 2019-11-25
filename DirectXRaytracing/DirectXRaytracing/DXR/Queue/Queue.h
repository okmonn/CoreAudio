#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12CommandQueue;
struct ID3D12Fence1;

class Queue
{
public:
	// �R���X�g���N�^
	Queue(const D3D12_COMMAND_LIST_TYPE& type);
	// �f�X�g���N�^
	~Queue();

	// �t�F���X�J�E���g�̃Z�b�g
	void Signal(ID3D12Fence1* fence, unsigned __int64& fenceCnt);

	// �R�}���h�L���[�̎擾
	ID3D12CommandQueue* Que(void) const;

private:
	// �R�}���h�L���[�̐���
	void CreateQueue(const D3D12_COMMAND_LIST_TYPE& type);

	// �R�}���h�L���[
	ID3D12CommandQueue* queue;
};
