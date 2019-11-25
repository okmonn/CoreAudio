#include "Swap.h"
#include "../Device/Device.h"
#include "../Window/Window.h"
#include "../Queue/Queue.h"
#include <d3d12.h>
#include <dxgi1_6.h>

// コンストラクタ
Swap::Swap(std::weak_ptr<Window>win, std::weak_ptr<Queue>queue, const unsigned int& bufferCnt) : 
	swap(nullptr)
{
	CreateSwap(win, queue, bufferCnt);
}

// デストラクタ
Swap::~Swap()
{
	if (swap != nullptr)
	{
		swap->Release();
		swap = nullptr;
	}
}

// スワップチェインの生成
void Swap::CreateSwap(std::weak_ptr<Window>win, std::weak_ptr<Queue>queue, const unsigned int& bufferCnt)
{
	Vec2 winSize = win.lock()->WinSize();

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.AlphaMode   = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.BufferCount = bufferCnt;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Flags       = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.Format      = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height      = winSize.y;
	desc.SampleDesc  = { 1, 0 };
	desc.Scaling     = DXGI_SCALING::DXGI_SCALING_STRETCH;
	desc.Stereo      = false;
	desc.SwapEffect  = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Width       = winSize.x;
	auto hr = Device::Get().Factory()->CreateSwapChainForHwnd(queue.lock()->Que(), win.lock()->Win(), &desc, nullptr, nullptr, (IDXGISwapChain1**)&swap);
	_ASSERT(hr == S_OK);
}

// 画面のバッファ切り替え
void Swap::Present(const unsigned int& interval, const unsigned int& flag)
{
	auto hr = swap->Present(interval, flag);
	_ASSERT(hr == S_OK);
}

// スワップチェインの取得
IDXGISwapChain4* Swap::Swp(void) const
{
	return swap;
}

// バックバッファ数の取得
unsigned int Swap::Buffer(void) const
{
	DXGI_SWAP_CHAIN_DESC1 desc{};
	auto hr = swap->GetDesc1(&desc);
	_ASSERT(hr == S_OK);

	return desc.BufferCount;
}

// バックバッファ番号の取得
unsigned int Swap::CurrentIndex(void) const
{
	return swap->GetCurrentBackBufferIndex();
}
