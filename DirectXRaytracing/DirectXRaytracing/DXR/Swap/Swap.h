#pragma once
#include <memory>

struct IDXGISwapChain4;
class Window;
class Queue;

class Swap
{
public:
	// コンストラクタ
	Swap(std::weak_ptr<Window>win, std::weak_ptr<Queue>queue, const unsigned int& bufferCnt = 2);
	// デストラクタ
	~Swap();

	// 画面のバッファ切り替え
	void Present(const unsigned int& interval = 0, const unsigned int& flag = 0);

	// スワップチェインの取得
	IDXGISwapChain4* Swp(void) const;

	// バックバッファ数の取得
	unsigned int Buffer(void) const;

	// バックバッファ番号の取得
	unsigned int CurrentIndex(void) const;
	
private:
	// スワップチェインの生成
	void CreateSwap(std::weak_ptr<Window>win, std::weak_ptr<Queue>queue, const unsigned int& bufferCnt);


	// スワップチェイン
	IDXGISwapChain4* swap;
};
