#pragma once
#include "../etc/Unknown.h"
#include <array>
#include <thread>

class Manager;

class __declspec(uuid("00000000-0917-0917-0917-000000000000"))
AudioEngin : public MyUnknown<AudioEngin>
{
	friend Manager;
public:

private:
	// �R���X�g���N�^
	AudioEngin();
	// �f�X�g���N�^
	~AudioEngin();

	// �񓯊�����
	void Stream(void);


	// �C�x���g
	std::array<void*, 2>handle;

	// �X���b�h
	std::thread th;
};
