#ifndef DIRECTX_DEPTH_STENCIL
#define DIRECTX_DEPTH_STENCIL

#include "DescriptorHeap.h"
#include "Vector.h"

struct DepthStencil :
	public BaseClass<ID3D12Resource1> {
public:
	/*コンストラクタ*/
	DepthStencil();
	/*コンストラクタ
	.Device
	.ウィンドウサイズ*/
	DepthStencil(const Device& device, const okmonn::Vec2& window_size);

public:
	/*DSVの生成
	.Device
	.ウィンドウサイズ*/
	void CreateDepthStencilView(const Device& device, const okmonn::Vec2& window_size);

private:
	/*リソースの生成
	.Device
	.ウィンドウサイズ*/
	void CreateRsc(const Device& device, const okmonn::Vec2& window_size);

public:
	/*ヒープ*/
	DescriptorHeap heap;
};

#endif
