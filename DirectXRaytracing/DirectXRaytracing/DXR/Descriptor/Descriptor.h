#pragma once
#include <vector>

enum D3D12_DESCRIPTOR_HEAP_TYPE : int;
enum D3D12_HEAP_FLAGS: int;
enum D3D12_RESOURCE_STATES : int;
struct ID3D12DescriptorHeap;
struct D3D12_HEAP_PROPERTIES;
struct D3D12_RESOURCE_DESC;
struct ID3D12Resource;
struct D3D12_CLEAR_VALUE;


class Descriptor
{
public:
	// コンストラクタ
	Descriptor();
	// デストラクタ
	virtual ~Descriptor();

	// マップ
	void Map(const size_t& index, void** buf);

	// アンマップ
	void UnMap(const unsigned int& index);

	// ヒープの取得
	ID3D12DescriptorHeap* Heap(void) const;

protected:
	// デフォルトヒーププロパティの取得
	static D3D12_HEAP_PROPERTIES DefaultProp(void);

	// アップロードヒーププロパティの取得
	static D3D12_HEAP_PROPERTIES UploadProp(void);

	// ヒープの生成
	void CreateHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& type, const bool& shaderFlag = false);

	// リソースの生成
	void CreateRsc(ID3D12Resource** rsc, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state,
		const D3D12_HEAP_FLAGS& flag = D3D12_HEAP_FLAGS(0), const D3D12_CLEAR_VALUE* clear = nullptr);

	// RTVの生成
	void RTV(const size_t& index);

	// UAVの生成
	void UAV(const unsigned int& index, const unsigned int& num, const unsigned int& stride);


	// ヒープ
	ID3D12DescriptorHeap* heap;

	// リソース
	std::vector<ID3D12Resource*>rsc;
};