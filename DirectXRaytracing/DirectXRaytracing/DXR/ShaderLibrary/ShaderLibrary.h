#pragma once
#include <string>
#include <memory>
#include <unordered_map>

struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct D3D12_STATE_SUBOBJECT;

class ShaderLibrary
{
public:
	// �R���p�C��
	void Compile(const std::string& fileName, std::string& entry, const std::initializer_list<std::string>& func, const std::string& ver);

	// �T�u�I�u�W�F�N�g�̎擾
	D3D12_STATE_SUBOBJECT Sub(const std::string& fileName);

	// �C���X�^���X�ϐ��̎擾
	static ShaderLibrary& Get(void);

private:
	ShaderLibrary(const ShaderLibrary&) = delete;
	void operator=(const ShaderLibrary&) = delete;

	// �R���X�g���N�^
	ShaderLibrary();
	// �f�X�g���N�^
	~ShaderLibrary();

	// �}���`�o�C�g���������j�R�[�h�����ɕϊ�
	std::wstring ChangeCode(const std::string& str);

	// �R���p�C��
	void Compile(const std::string& fileName, const std::string& entry, const std::string& ver, ID3DBlob** blob);


	// �T�u�I�u�W�F�N�g
	std::unordered_map<std::string, std::unique_ptr<D3D12_STATE_SUBOBJECT>>sub;
};
