#ifndef DIRECTX_CONSTANT_BUFFER
#define DIRECTX_CONSTANT_BUFFER

#include "DescriptorHeap.h"
#include <vector>

struct ConstantBuffer : 
	public BaseClass<ID3D12Resource1> {
public:
	/*コンストラクタ*/
	ConstantBuffer();
	/*コンストラクタ
	.Device
	.データサイズ*/
	ConstantBuffer(const Device& device, const std::uint32_t& data_size);

public:
	/*CBVの生成
	.Device
	.データサイズ*/
	void CreateConstantBufferView(const Device& device, const std::uint32_t& data_size);
	/*定数データの更新
	.定数データ
	.データサイズ*/
	void UpDateConstant(const void* data, const std::uint32_t& data_size);

private:
	/*リソースの生成
	.Device
	.データサイズ*/
	void CreateRsc(const Device& device, const std::uint32_t& data_size);

public:
	/*ヒープ*/
	DescriptorHeap heap;
};

#endif
