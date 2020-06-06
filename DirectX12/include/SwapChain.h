#ifndef DIRECTX_SWAP_CHAIN
#define DIRECTX_SWAP_CHAIN

#include "BaseClass.h"
#include <cstdint>

class Window;
struct CommandQueue;

struct SwapChain :
	public BaseClass<IDXGISwapChain4> {
public:
	/*コンストラクタ
	.Window
	.CommandQueue
	.バックバッファ数*/
	SwapChain(const Window& window, const CommandQueue& command_queue,
		const std::uint32_t& back_buffer_num = 2);

public:
	/*表・裏バッファの切り替え*/
	void Present(void) const;

public:
	/*バックバッファ数の取得
	return バックバッファ数*/
	std::uint32_t GetBackBufferNum(void) const;

private:
	/*スワップチェインの生成
	.Window
	.CommandQueue*/
	void CreateSwapChain(const Window& window, const CommandQueue& command_queue);

private:
	/*バックバッファ数*/
	std::uint32_t back_buffer_num;
};

#endif
