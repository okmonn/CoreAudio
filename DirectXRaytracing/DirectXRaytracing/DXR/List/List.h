#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12GraphicsCommandList5;

class List
{
public:
	// コンストラクタ
	List(const D3D12_COMMAND_LIST_TYPE& type);
	// デストラクタ
	~List();

	// コマンドリストの取得
	ID3D12GraphicsCommandList5* Lis(void) const;
	
private:
	// コマンドリストの生成
	void CreateList(const D3D12_COMMAND_LIST_TYPE& type);

	// コマンドリスト
	ID3D12GraphicsCommandList5* list;
};
