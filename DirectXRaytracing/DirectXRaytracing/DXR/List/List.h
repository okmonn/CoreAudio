#pragma once
#include <memory>

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12GraphicsCommandList5;
struct ID3D12PipelineState;
struct D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC;
struct ID3D12Resource;
class Allocator;

class List
{
public:
	// �R���X�g���N�^
	List(const D3D12_COMMAND_LIST_TYPE& type);
	// �f�X�g���N�^
	~List();

	// �R�}���h���X�g�̃��Z�b�g
	void Reset(std::weak_ptr<Allocator>allo, ID3D12PipelineState* pipe = nullptr);

	// �R�}���h���X�g�����
	void Close(void);

	// ���z�\���̃r���h
	void AccelerationBuild(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& buildDesc);

	// UAV�o���A
	void Barrier(ID3D12Resource* rsc);

	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList5* Lis(void) const;
	
private:
	// �R�}���h���X�g�̐���
	void CreateList(const D3D12_COMMAND_LIST_TYPE& type);

	// �R�}���h���X�g
	ID3D12GraphicsCommandList5* list;
};
