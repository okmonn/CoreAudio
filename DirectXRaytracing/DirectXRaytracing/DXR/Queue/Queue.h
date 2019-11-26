#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12CommandQueue;
struct ID3D12Fence1;
struct ID3D12CommandList;

class Queue
{
public:
	// �R���X�g���N�^
	Queue(const D3D12_COMMAND_LIST_TYPE& type);
	// �f�X�g���N�^
	~Queue();

	// �R�}���h���X�g�̎��s
	void Execution(ID3D12CommandList* const* list, const size_t& listNum = 1);

	// �R�}���h�L���[�̎擾
	ID3D12CommandQueue* Que(void) const;

private:
	// �R�}���h�L���[�̐���
	void CreateQueue(const D3D12_COMMAND_LIST_TYPE& type);

	// �R�}���h�L���[
	ID3D12CommandQueue* queue;
};
