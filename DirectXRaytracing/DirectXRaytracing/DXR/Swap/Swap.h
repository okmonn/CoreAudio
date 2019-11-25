#pragma once
#include <memory>

struct IDXGISwapChain4;
class Window;
class Queue;

class Swap
{
public:
	// �R���X�g���N�^
	Swap(std::weak_ptr<Window>win, std::weak_ptr<Queue>queue, const unsigned int& bufferCnt = 2);
	// �f�X�g���N�^
	~Swap();

	// ��ʂ̃o�b�t�@�؂�ւ�
	void Present(const unsigned int& interval = 0, const unsigned int& flag = 0);

	// �X���b�v�`�F�C���̎擾
	IDXGISwapChain4* Swp(void) const;

	// �o�b�N�o�b�t�@���̎擾
	unsigned int Buffer(void) const;

	// �o�b�N�o�b�t�@�ԍ��̎擾
	unsigned int CurrentIndex(void) const;
	
private:
	// �X���b�v�`�F�C���̐���
	void CreateSwap(std::weak_ptr<Window>win, std::weak_ptr<Queue>queue, const unsigned int& bufferCnt);


	// �X���b�v�`�F�C��
	IDXGISwapChain4* swap;
};
