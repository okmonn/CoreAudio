#ifndef DIRECTX_INDEX_BUFFER
#define DIRECTX_INDEX_BUFFER

#include "BaseClass.h"
#include <cstdint>

struct Device;

struct IndexBuffer :
	public BaseClass<ID3D12Resource1> {
public:
	/*コンストラクタ
	.Device
	.バッファサイズ*/
	IndexBuffer(const Device& device, const std::uint32_t& buffer_size);

public:
	/*インデックスデータの更新
	.インデックスデータ*/
	void UpDateIndex(const void* data);

private:
	/*リソースの生成
	.Device
	.バッファサイズ*/
	void CraeteRsc(const Device& device, const std::uint32_t& buffer_size);
};

#endif
