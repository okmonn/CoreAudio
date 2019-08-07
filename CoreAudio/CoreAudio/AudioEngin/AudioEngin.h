#pragma once
#include "../etc/Unknown.h"
#include "../etc/Info.h"
#include <wrl.h>
#include <array>
#include <vector>
#include <memory>
#include <thread>

struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;
class Manager;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
AudioEngin : public MyUnknown<AudioEngin>
{
	friend Manager;
public:
	// ������
	long Initialize(const okmonn::AudioType& type, const okmonn::Info* info = nullptr);

	// �G���W���J�n
	long Start(void);

	// �G���W����~
	long Stop(void);

private:
	// �R���X�g���N�^
	AudioEngin(const unsigned int& index, const okmonn::DeviceType& type);
	// �f�X�g���N�^
	~AudioEngin();

	// �G���h�|�C���g�f�o�C�X����
	void CreateDevice(const unsigned int& index, const okmonn::DeviceType& type);

	// �I�[�f�B�I�N���C�A���g����
	long CreateAudio(const okmonn::AudioType& type, void* addr);

	// �I�[�f�B�I�����_���[����
	long CreateRender(void);

	// �񓯊�����
	void Stream(void);


	// �G���h�|�C���g�f�o�C�X
	Microsoft::WRL::ComPtr<IMMDevice>device;

	// �I�[�f�B�I�N���C�A���g
	Microsoft::WRL::ComPtr<IAudioClient>audio;

	// �I�[�f�B�I�����_���[
	IAudioRenderClient* render;

	// �T�E���h���
	okmonn::Info info;

	// �G���h�|�C���g�f�o�C�X�^�C�v
	okmonn::DeviceType deviceType;

	// �I�[�f�B�I�^�C�v
	okmonn::AudioType audioType;

	// �C�x���g
	std::array<void*, 2>handle;

	// �X���b�h
	std::thread th;

	std::shared_ptr<std::vector<unsigned char>>wave;
	okmonn::Info waveInfo;
};
