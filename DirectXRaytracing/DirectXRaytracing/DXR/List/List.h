#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12GraphicsCommandList5;

class List
{
public:
	// �R���X�g���N�^
	List(const D3D12_COMMAND_LIST_TYPE& type);
	// �f�X�g���N�^
	~List();

	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList5* Lis(void) const;
	
private:
	// �R�}���h���X�g�̐���
	void CreateList(const D3D12_COMMAND_LIST_TYPE& type);

	// �R�}���h���X�g
	ID3D12GraphicsCommandList5* list;
};
