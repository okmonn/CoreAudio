#pragma once
#include "../Descriptor/Descriptor.h"
#include "../Vector3.h"

class Primitive : 
	public Descriptor
{
public:
	// コンストラクタ
	Primitive(const Vec3f* vertex, const size_t& vertexNum);
	// デストラク
	~Primitive();

	// 頂点リソースの取得
	ID3D12Resource* Rsc(void) const;

private:
	// リソースの生成
	void CreateRsc(const Vec3f* vertex, const size_t& vertexNum);
};
