#pragma once
#include "Base/Unknown.h"
#include "Function/Function.h"
#include <list>
#include <wrl.h>
#include <array>
#include <mutex>
#include <thread>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
class Okdio;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
	OkdioEngine : 
	public okmonn::Unknown<OkdioEngine>
{
public:
	// �R���X�g���N�^
	OkdioEngine(const okmonn::AudioDevType& devType, const okmonn::AudioType& audioType, const int& devIndex);
	// �f�X�g���N�^
	~OkdioEngine();

	// Okdio����
	bool CreateOkdio(const std::string& fileName, const GUID& id, void** okdio);

	// �J�n
	bool Start(void);

	// ��~
	bool Stop(void);

private:
	// �I�[�f�B�I�f�o�C�X����
	void CreateDevice(const int& devIndex);

	// �I�[�f�B�I�N���C�A���g����
	void CreateAudio(void);

	// �t�H�[�}�b�g�擾
	void* GetFormat(void);

	// �I�[�f�B�I�N���C�A���g������
	void Initialized(void);

	// �I�[�f�B�I�����_���[����
	void CreateRender(void);

	// �񓯊�����
	void Stream(void);


	// �I�[�f�B�I�f�o�C�X�^�C�v
	okmonn::AudioDevType devType;

	// �I�[�f�B�I�^�C�v
	okmonn::AudioType audioType;

	// �T�E���h���
	okmonn::SoundInfo info;

	// �r������
	std::mutex mtx;

	// �X���b�h
	std::thread th;

	// �I�[�f�B�I�f�o�C�X
	Microsoft::WRL::ComPtr<IMMDevice>dev;

	// �I�[�f�B�I�N���C�A���g
	Microsoft::WRL::ComPtr<IAudioClient>audio;

	// �I�[�f�B�I�����_���[
	Microsoft::WRL::ComPtr<IAudioRenderClient>render;

	// �C�x���g�n���h��
	std::array<void*, 2>handle;
};
