#include "Association.h"
#include "../Helper.h"
#include <vector>
#include <d3d12.h>

// コンストラクタ
Association::Association(const std::initializer_list<std::string>& func, D3D12_STATE_SUBOBJECT* sub) :
	sub(std::make_unique<D3D12_STATE_SUBOBJECT>())
{
	std::vector<LPCWSTR>tmp;
	for (auto itr = func.begin(); itr != func.end(); ++itr)
	{
		tmp.push_back(DXR::ChangeCode(*itr).c_str());
	}

	D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION expo{};
	expo.NumExports            = func.size();
	expo.pExports              = tmp.data();
	expo.pSubobjectToAssociate = sub;

	this->sub->pDesc = &expo;
	this->sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
}

// デストラクタ
Association::~Association()
{
}

// サブオブジェクトの取得
D3D12_STATE_SUBOBJECT Association::Sub(void) const
{
	return *sub;
}
