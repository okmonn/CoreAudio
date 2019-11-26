#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12CommandAllocator;

class Allocator
{
public:
	// コンストラクタ
	Allocator(const D3D12_COMMAND_LIST_TYPE& type);
	// デストラクタ
	~Allocator();

	// コマンドアロケータのリセット
	void Reset(void);

	// コマンドアロケータの取得
	ID3D12CommandAllocator* Allo(void) const;

private:
	// コマンドアロケータの生成
	void CreateAllo(const D3D12_COMMAND_LIST_TYPE& type);

	// コマンドアロケータ
	ID3D12CommandAllocator* allo;
};
