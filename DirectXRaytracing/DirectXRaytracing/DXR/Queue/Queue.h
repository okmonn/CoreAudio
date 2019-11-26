#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12CommandQueue;
struct ID3D12Fence1;
struct ID3D12CommandList;

class Queue
{
public:
	// コンストラクタ
	Queue(const D3D12_COMMAND_LIST_TYPE& type);
	// デストラクタ
	~Queue();

	// コマンドリストの実行
	void Execution(ID3D12CommandList* const* list, const size_t& listNum = 1);

	// コマンドキューの取得
	ID3D12CommandQueue* Que(void) const;

private:
	// コマンドキューの生成
	void CreateQueue(const D3D12_COMMAND_LIST_TYPE& type);

	// コマンドキュー
	ID3D12CommandQueue* queue;
};
