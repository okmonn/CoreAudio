#include "ClosestHit.h"
#include "../Helper.h"
#include <d3d12.h>

// コンストラクタ
ClosestHit::ClosestHit(const std::string& anyHit, const std::string& closestHit, const std::string& hitGroup) : 
	sub(std::make_unique<D3D12_STATE_SUBOBJECT>())
{
	D3D12_HIT_GROUP_DESC desc{};
	desc.AnyHitShaderImport     = DXR::ChangeCode(anyHit).c_str();
	desc.ClosestHitShaderImport = DXR::ChangeCode(closestHit).c_str();
	desc.HitGroupExport         = DXR::ChangeCode(hitGroup).c_str();

	sub->pDesc = &desc;
	sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
}

// デストラク
ClosestHit::~ClosestHit()
{
}

// サブオブジェクトの取得
D3D12_STATE_SUBOBJECT ClosestHit::Sub(void) const
{
	return *sub;
}
