#pragma once
#include <vector>

enum D3D12_DESCRIPTOR_HEAP_TYPE : int;
enum D3D12_RESOURCE_DIMENSION : int;
enum D3D12_HEAP_FLAGS: int;
enum D3D12_RESOURCE_STATES : int;
struct ID3D12DescriptorHeap;
struct D3D12_HEAP_PROPERTIES;
struct D3D12_RESOURCE_DESC;
struct ID3D12Resource;
struct D3D12_CLEAR_VALUE;

namespace Descriptor
{
	// ヒープの生成
	void CreateHeap(ID3D12DescriptorHeap** heap, const D3D12_DESCRIPTOR_HEAP_TYPE& type, const unsigned int& rscNum, const bool& shaderFlag = false);

	// デフォルトヒーププロパティの取得
	D3D12_HEAP_PROPERTIES DefaultProp(void);

	// アップロードヒーププロパティの取得
	D3D12_HEAP_PROPERTIES UploadProp(void);

	// リソースの生成
	void CreateRsc(ID3D12Resource** rsc, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state, 
		const D3D12_HEAP_FLAGS& flag = D3D12_HEAP_FLAGS(0), const D3D12_CLEAR_VALUE* clear = nullptr);
}


class Descriptor
{
public:
	// コンストラクタ
	Descriptor();
	// デストラクタ
	virtual ~Descriptor();

protected:
	// ヒープの生成
	void CreateHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& type, const unsigned int& rscNum, const bool& shaderFlag = false);

	// リソースの生成
	void CreateRsc(ID3D12Resource** rsc, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state,
		const D3D12_HEAP_FLAGS& flag = D3D12_HEAP_FLAGS(0), const D3D12_CLEAR_VALUE* clear = nullptr);


	// ヒープ
	ID3D12DescriptorHeap* heap;

	// リソース
	std::vector<ID3D12Resource*>rsc;
};