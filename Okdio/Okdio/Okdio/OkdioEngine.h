#pragma once
#include "ClassBase/UnKnown.h"
#include "Information.h"
#include "Function/Function.h"
#include <list>
#include <wrl.h>
#include <array>
#include <mutex>
#include <vector>
#include <thread>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
class Okdio;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
	OkdioEngine : public UnKnown<OkdioEngine>
{
public:
	// �R���X�g���N�^
	OkdioEngine(const okmonn::AudioDeviceType& devType, const okmonn::AudioType& audioType, const char& deviceIndex = -1);
	// �f�X�g���N�^
	~OkdioEngine();

	// Okdio����
	long CreateOkdio(const std::string& fileName, const GUID& id, void** obj);

	// �J�n
	void Start(void);

	// ��~
	void Stop(void);

private:
	void operator=(const OkdioEngine&) = delete;

	// �I�[�f�B�I�f�o�C�X����
	void CreateDevice(const char& deviceIndex);

	// �t�H�[�}�b�g�ݒ�
	void* SetFormat(IAudioClient* audio, const okmonn::AudioInfo* info = nullptr);

	// �I�[�f�B�I�N���C�A���g����
	void CreateAudioClient(void);

	// �I�[�f�B�I�N���C�A���g������
	void Initialize(void);

	// �I�[�f�B�I�����_���[����
	void CreateAudioRender(void);

	// �񓯊�����
	void Stream(void);

	void TTT(std::vector<float>& data);


	// �I�[�f�B�I�f�o�C�X
	Microsoft::WRL::ComPtr<IMMDevice>device;

	// �I�[�f�B�I�N���C�A���g
	Microsoft::WRL::ComPtr<IAudioClient>audio;

	// �I�[�f�B�I�����_���[
	Microsoft::WRL::ComPtr<IAudioRenderClient>render;

	// �I�[�f�B�I�f�o�C�X�^�C�v
	okmonn::AudioDeviceType devType;

	// �I�[�f�B�I�^�C�v
	okmonn::AudioType audioType;

	// �I�[�f�B�I���
	okmonn::AudioInfo info;

	// �r������
	std::mutex mtx;

	// �X���b�h
	std::thread th;

	// �C�x���g�n���h��
	std::array<void*, 2>handle;

	// Okdio���X�g
	std::list<Okdio*>okdio;
};
