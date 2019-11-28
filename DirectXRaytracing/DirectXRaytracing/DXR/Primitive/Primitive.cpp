#include "Primitive.h"
#include "../Constant/Constant.h"
#include <d3d12.h>

// コンストラクタ
Primitive::Primitive(const Vec3f* vertex, const size_t& vertexNum, const size_t& constantSize, const size_t& instanceNum) : 
	instanceNum(instanceNum)
{
	buf.resize(instanceNum);
	matrix.resize(instanceNum);
	CreateRsc(vertex, vertexNum);
	constant = std::make_unique<Constant>(constantSize, instanceNum);
}

// デストラク
Primitive::~Primitive()
{
	for (auto& i : rsc)
	{
		if (i != nullptr)
		{
			i->Release();
			i = nullptr;
		}
	}
}

// 定数のセット
void Primitive::SetConstant(const size_t& instanceID, void* data)
{
	std::memcpy(buf[instanceID], data, Const(instanceID)->GetDesc().Width);
}

// リソースの生成
void Primitive::CreateRsc(const Vec3f* vertex, const size_t& vertexNum)
{
	D3D12_RESOURCE_DESC desc{};
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	desc.Height           = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = sizeof(vertex[0]) * vertexNum;
	Descriptor::CreateRsc(&(*rsc.begin()), UploadProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ);

	void* buf = nullptr;
	Map(0, &buf);
	std::memcpy(buf, vertex, desc.Width);
	UnMap(0);
}

// 平行移動
void Primitive::Translation(const size_t& instanceID, const Vec3f& vec)
{
	matrix[instanceID].Translation(vec);
}

// インスタンス数の取得
size_t Primitive::InstanceNum(void) const
{
	return instanceNum;
}

// 頂点リソースの取得
ID3D12Resource* Primitive::Vertex(void) const
{
	return *rsc.begin();
}

// 定数リソースの取得
ID3D12Resource* Primitive::Const(const size_t& instanceID) const
{
	return constant->Rsc[instanceID];
}

// 行列の取得
Matrix3x4 Primitive::Matrix(const size_t& instanceID)
{
	return matrix[instanceID];
}
