#ifndef DIRECTX_RENDER_TARGET
#define DIRECTX_RENDER_TARGET

#include "DescriptorHeap.h"
#include <vector>

struct Device;
struct CommandList;
struct SwapChain;

struct RenderTarget {
public:
	/*コンストラクタ
	.Device
	.SwapChain*/
	RenderTarget(const Device& device, const SwapChain& swap_chain);

public:
	/*現在のリソースの取得
	return リソース*/
	ID3D12Resource1* Get(void) const;
	/*現在のバックバッファ番号の取得
	return 現在のバックバッファ*/
	std::uint32_t GetBackBufferIndex(void) const;

private:
	/*リソースの取得
	.SwapChain*/
	void GetRsc(const SwapChain& swap_chain);
	/*RTVの生成
	.Device*/
	void CreateRenderTargetView(const Device& device);

public:
	/*ヒープ*/
	DescriptorHeap heap;

private:
	/*スワップチェイン*/
	const SwapChain* swap_chain;

private:
	/*リソース*/
	std::vector<ComPtr<ID3D12Resource1>>rsc;
};

#endif
