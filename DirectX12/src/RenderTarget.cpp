#include "../include/RenderTarget.h"
#include "../include/Device.h"
#include "../include/CommandList.h"
#include "../include/SwapChain.h"
#include <cassert>

RenderTarget::RenderTarget(const Device& device, const SwapChain& swap_chain) : 
	swap_chain(&swap_chain), rsc(swap_chain.GetBackBufferNum())
{
	heap.CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV, swap_chain.GetBackBufferNum());
	GetRsc(swap_chain);
	CreateRenderTargetView(device);
}

void RenderTarget::GetRsc(const SwapChain& swap_chain)
{
	for (std::uint32_t i = 0; i < rsc.size(); ++i) {
		auto hr = swap_chain.Get()->GetBuffer(i, IID_PPV_ARGS(&rsc[i]));
		assert(hr == S_OK);
	}
}

void RenderTarget::CreateRenderTargetView(const Device& device)
{
	for (std::uint32_t i = 0; i < rsc.size(); ++i) {
		D3D12_RENDER_TARGET_VIEW_DESC desc{};
		desc.Format        = rsc[i]->GetDesc().Format;
		desc.Texture2D     = {};
		desc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;

		auto handle = heap.Get()->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += device.Get()->GetDescriptorHandleIncrementSize(heap.Get()->GetDesc().Type) * i;
		device.Get()->CreateRenderTargetView(rsc[i].Get(), &desc, handle);
	}
}

ID3D12Resource1* RenderTarget::Get(void) const
{
	return rsc[swap_chain->Get()->GetCurrentBackBufferIndex()].Get();
}

std::uint32_t RenderTarget::GetBackBufferIndex(void) const
{
	return swap_chain->Get()->GetCurrentBackBufferIndex();
}
