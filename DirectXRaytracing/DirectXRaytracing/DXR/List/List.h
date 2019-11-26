#pragma once
#include <memory>

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12GraphicsCommandList5;
struct ID3D12PipelineState;
struct D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC;
struct ID3D12Resource;
class Allocator;

class List
{
public:
	// コンストラクタ
	List(const D3D12_COMMAND_LIST_TYPE& type);
	// デストラクタ
	~List();

	// コマンドリストのリセット
	void Reset(std::weak_ptr<Allocator>allo, ID3D12PipelineState* pipe = nullptr);

	// コマンドリストを閉じる
	void Close(void);

	// 仮想構造のビルド
	void AccelerationBuild(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& buildDesc);

	// UAVバリア
	void Barrier(ID3D12Resource* rsc);

	// コマンドリストの取得
	ID3D12GraphicsCommandList5* Lis(void) const;
	
private:
	// コマンドリストの生成
	void CreateList(const D3D12_COMMAND_LIST_TYPE& type);

	// コマンドリスト
	ID3D12GraphicsCommandList5* list;
};
