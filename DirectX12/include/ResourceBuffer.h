#ifndef DIRECTX_RESOURCE_BUFFER
#define DIRECTX_RESOURCE_BUFFER

#include "DescriptorHeap.h"

struct Device;

struct ResourceBuffer :
	public BaseClass<ID3D12Resource1> {
public:
	/*コンストラクタ*/
	ResourceBuffer();
	/*コンストラクタ
	.Device
	.横幅
	.縦幅*/
	ResourceBuffer(const Device& device, const std::uint32_t& width, const std::uint32_t& height);

public:
	/*SRVの生成
	.Device
	.横幅
	.縦幅*/
	void CreateShaderResourceView(const Device& device, const std::uint32_t& width, const std::uint32_t& height);
	/*リソースデータの更新
	.リソースデータ*/
	void UpDateResource(const void* data);

private:
	/*リソースの生成
	.Device
	.横幅
	.縦幅*/
	void CreateRsc(const Device& device, const std::uint32_t& width, const std::uint32_t& height);

public:
	/*ヒープ*/
	DescriptorHeap heap;
};

#endif
