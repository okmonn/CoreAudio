#pragma once
#include <string>
#include <memory>

struct D3D12_STATE_SUBOBJECT;

class Association
{
public:
	// コンストラクタ
	Association(const std::initializer_list<std::string>& func, D3D12_STATE_SUBOBJECT* sub);
	// デストラクタ
	~Association();

	// サブオブジェクトの取得
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// サブオブジェクト
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;
};
