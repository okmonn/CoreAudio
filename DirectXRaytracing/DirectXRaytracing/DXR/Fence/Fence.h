#pragma once
#include <memory>

struct ID3D12Fence1;
class Queue;

class Fence
{
public:
	// コンストラクタ
	Fence(std::weak_ptr<Queue>queue);
	// デストラクタ
	~Fence();

	// コマンド待機
	void Wait(void);

	// フェンスの取得
	ID3D12Fence1* Fen(void) const;

private:
	// フェンスの生成
	void CreateFence(void);


	// キュー
	std::weak_ptr<Queue>queue;

	// フェンス
	ID3D12Fence1* fence;

	// フェンスカウント
	unsigned __int64 fenceCnt;

	// フェンスイベント
	void* eventHandle;
};
