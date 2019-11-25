#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

class Window;

class Output : 
	public Descriptor
{
public:
	// �R���X�g���N�^
	Output(std::weak_ptr<Window>win);
	// �f�X�g���N�^
	~Output();

	// ���\�[�X�̎擾
	ID3D12Resource* Rsc(void) const;

private:
	// ���\�[�X����
	void CreateRsc(std::weak_ptr<Window>win);
};
