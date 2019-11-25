#pragma once

enum D3D12_COMMAND_LIST_TYPE : int;
struct ID3D12CommandQueue;
struct ID3D12Fence1;

class Queue
{
public:
	// コンストラクタ
	Queue(const D3D12_COMMAND_LIST_TYPE& type);
	// デストラクタ
	~Queue();

	// フェンスカウントのセット
	void Signal(ID3D12Fence1* fence, unsigned __int64& fenceCnt);

	// コマンドキューの取得
	ID3D12CommandQueue* Que(void) const;

private:
	// コマンドキューの生成
	void CreateQueue(const D3D12_COMMAND_LIST_TYPE& type);

	// コマンドキュー
	ID3D12CommandQueue* queue;
};
