#include "../include/SwapChain.h"
#include "../include/Window.h"
#include "../include/CommandQueue.h"
#include <cassert>

SwapChain::SwapChain(const Window& window, const CommandQueue& command_queue, const std::uint32_t& back_buffer_num) : 
	back_buffer_num(back_buffer_num)
{
	CreateSwapChain(window, command_queue);
}

void SwapChain::CreateSwapChain(const Window& window, const CommandQueue& command_queue)
{
	ComPtr<IDXGIFactory7>factory;
	std::uint32_t debug_flag = 0U;
#ifdef _DEBUG
	debug_flag = DXGI_CREATE_FACTORY_DEBUG;
#endif
	auto hr = CreateDXGIFactory2(debug_flag, IID_PPV_ARGS(&factory));
	assert(hr == S_OK);

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.AlphaMode   = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.BufferCount = back_buffer_num;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Flags       = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.Format      = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height      = window.GetWindowSize().y;
	desc.SampleDesc  = { 1, 0 };
	desc.Scaling     = DXGI_SCALING::DXGI_SCALING_STRETCH;
	desc.Stereo      = false;
	desc.SwapEffect  = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Width       = window.GetWindowSize().x;

	hr = factory->CreateSwapChainForHwnd(command_queue.Get(), window.Get(), &desc, nullptr, nullptr, (IDXGISwapChain1**)GetAddress());
	assert(hr == S_OK);
}

void SwapChain::Present(void) const
{
	Get()->Present(1, 0);
}

std::uint32_t SwapChain::GetBackBufferNum(void) const
{
	return back_buffer_num;
}
