#pragma once

struct IDXGIFactory7;
struct ID3D12Device5;

class Device
{
public:
	// �t�@�N�g���[�̎擾
	IDXGIFactory7* Factory(void) const;

	// �f�o�C�X�̎擾
	ID3D12Device5* Dev(void) const;

	// �C���X�^���X�ϐ��̎擾
	static Device& Get(void);

private:
	Device(const Device&) = delete;
	void operator=(const Device&) = delete;

	// �R���X�g���N�^
	Device();
	// �f�X�g���N�^
	~Device();

	// �t�@�N�g���[�̐���
	void CreateFactory(void);

	// �f�o�C�X�̐���
	void CreateDevice(void);


	// �t�@�N�g���[
	IDXGIFactory7* factory;

	// �f�o�C�X
	ID3D12Device5* device;
};
