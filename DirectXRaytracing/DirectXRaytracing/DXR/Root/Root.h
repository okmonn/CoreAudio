#pragma once
#include <string>
#include <memory>

struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct D3D12_STATE_SUBOBJECT;
struct ID3D12RootSignature;

class Root
{
public:
	// �R���X�g���N�^
	Root();
	Root(const std::string& fileName, const std::string& entry, const std::initializer_list<std::string>& func, const std::string& ver);
	// �f�X�g���N�^
	~Root();

	// ���[�g�V�O�l�`���̎擾
	ID3D12RootSignature* Signature(void) const;

	//	�V�F�[�_�̃T�u�I�u�W�F�N�g�̎擾
	D3D12_STATE_SUBOBJECT Shader(void) const;

	// ���[�g�V�O�l�`���̃T�u�I�u�W�F�N�g�̎擾
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// ���[�g�V�O�l�`���̐���
	void CreateRoot(void);

	// ���[�g�V�O�l�`���̐���
	void CreateRoot(ID3DBlob* blob);

	// ���[�J�����[�g�V�O�l�`���̐���
	void CreateLocal(void);

	// �O���[�o�����[�g�V�O�l�`���̐���
	void CreateGlobal(void);


	//	�V�F�[�_�̃T�u�I�u�W�F�N�g
	std::unique_ptr<D3D12_STATE_SUBOBJECT>shader;

	// ���[�g�V�O�l�`���̃T�u�I�u�W�F�N�g
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;

	// ���[�g�V�O�l�`��
	ID3D12RootSignature* root;
};
