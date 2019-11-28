#pragma once
#include "../Descriptor/Descriptor.h"
#include "../Matrix.h"
#include <vector>
#include <memory>

class Constant;

class Primitive : 
	public Descriptor
{
public:
	// コンストラクタ
	Primitive(const Vec3f* vertex, const size_t& vertexNum, const size_t& constantSize, const size_t& instanceNum);
	// デストラク
	~Primitive();

	// 定数のセット
	void SetConstant(const size_t& instanceID, void* data);

	// 平行移動
	void Translation(const size_t& instanceID, const Vec3f& vec);

	// インスタンス数の取得
	size_t InstanceNum(void) const;

	// 頂点リソースの取得
	ID3D12Resource* Vertex(void) const;

	// 定数リソースの取得
	ID3D12Resource* Const(const size_t& instanceID) const;

	// 行列の取得
	Matrix3x4 Matrix(const size_t& instanceID);

private:
	// リソースの生成
	void CreateRsc(const Vec3f* vertex, const size_t& vertexNum);


	// コンスタント
	std::unique_ptr<Constant>constant;

	// インスタンス数
	size_t instanceNum;

	// 定数バッファ
	std::vector<void*>buf;

	// 行列
	std::vector<Matrix3x4>matrix;
};
