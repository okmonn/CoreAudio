#pragma once
#include <string>
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class ClosestHit
{
public:
	// コンストラクタ
	ClosestHit(const std::string& anyHit, const std::string& closestHit, const std::string& hitGroup);
	// デストラク
	~ClosestHit();

	// サブオブジェクトの取得
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// サブオブジェクト
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
