#ifndef DIRECTX_VERTEX_BUFFER
#define DIRECTX_VERTEX_BUFFER

#include "../include/BaseClass.h"
#include <cstdint>

struct Device;

struct VertexBuffer : 
	public BaseClass<ID3D12Resource1> {
public:
	/*コンストラクタ
	.Device
	.バッファサイズ*/
	VertexBuffer(const Device& device, const std::uint32_t& buffer_size);

public:
	/*頂点データの更新
	.頂点データ*/
	void UpDateVertex(const void* vertex_data);

private:
	/*リソースの生成
	.Device
	.バッファサイズ*/
	void CreateRsc(const Device& device, const std::uint32_t& buffer_size);
};

#endif
