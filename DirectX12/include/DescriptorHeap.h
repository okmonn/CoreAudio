#ifndef DIRECTX_DESCRIPTOR_HEAP
#define DIRECTX_DESCRIPTOR_HEAP

#include "BaseClass.h"
#include <cstdint>

struct Device;
struct CommandList;

struct DescriptorHeap :
	public BaseClass<ID3D12DescriptorHeap> {
public:
	/*コンストラクタ*/
	DescriptorHeap();

public:
	/*ヒープの生成
	.Device
	.ヒープ種別
	.リソース数
	.シェーダ利用フラグ*/
	void CreateHeap(const Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE& type, const std::uint32_t& rsc_num, const bool& shader_use_flag = false);
};

#endif
