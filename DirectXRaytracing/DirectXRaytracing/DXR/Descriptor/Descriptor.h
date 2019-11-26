#pragma once
#include <vector>

enum D3D12_DESCRIPTOR_HEAP_TYPE : int;
enum D3D12_HEAP_FLAGS: int;
enum D3D12_RESOURCE_STATES : int;
struct ID3D12DescriptorHeap;
struct D3D12_HEAP_PROPERTIES;
struct D3D12_RESOURCE_DESC;
struct ID3D12Resource;
struct D3D12_CLEAR_VALUE;


class Descriptor
{
public:
	// �R���X�g���N�^
	Descriptor();
	// �f�X�g���N�^
	virtual ~Descriptor();

	// �}�b�v
	void Map(const size_t& index, void** buf);

	// �A���}�b�v
	void UnMap(const unsigned int& index);

	// �q�[�v�̎擾
	ID3D12DescriptorHeap* Heap(void) const;

protected:
	// �f�t�H���g�q�[�v�v���p�e�B�̎擾
	static D3D12_HEAP_PROPERTIES DefaultProp(void);

	// �A�b�v���[�h�q�[�v�v���p�e�B�̎擾
	static D3D12_HEAP_PROPERTIES UploadProp(void);

	// �q�[�v�̐���
	void CreateHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& type, const bool& shaderFlag = false);

	// ���\�[�X�̐���
	void CreateRsc(ID3D12Resource** rsc, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state,
		const D3D12_HEAP_FLAGS& flag = D3D12_HEAP_FLAGS(0), const D3D12_CLEAR_VALUE* clear = nullptr);

	// RTV�̐���
	void RTV(const size_t& index);

	// UAV�̐���
	void UAV(const unsigned int& index, const unsigned int& num, const unsigned int& stride);


	// �q�[�v
	ID3D12DescriptorHeap* heap;

	// ���\�[�X
	std::vector<ID3D12Resource*>rsc;
};