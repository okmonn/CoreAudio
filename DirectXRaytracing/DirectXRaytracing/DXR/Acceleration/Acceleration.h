#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

struct D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS;
struct D3D12_RAYTRACING_INSTANCE_DESC;
class List;
class Primitive;
class Matrix3x4;

namespace DXR
{
	// 仮想構造のタイプ
	enum class AccelerationType
	{
		bottom = 2,
		top
	};
}

class Acceleration : 
	public Descriptor
{
public:
	// コンストラクタ
	Acceleration(std::weak_ptr<List>list, const DXR::AccelerationType& type);
	// デストラクタ
	~Acceleration();

	// トップレベルの生成
	void CreateTop(const size_t& rayNum, Acceleration* bottom, const size_t& bottomNum);

	// ボトムレベルの生成
	void CreateBottom(std::weak_ptr<Primitive>prim);

	// リザルトリソースの取得
	ID3D12Resource* Result(void) const;

	// スクラッチリソースの取得
	ID3D12Resource* Scratch(void) const;

	// インスタンスリソースの取得
	ID3D12Resource* Instance(void) const;

	// インスタンス数の取得
	size_t InstanceNum(void) const;

private:
	// ビルド
	void Build(void);

	// トップレベルの更新
	void UpData(void);


	// リスト
	std::weak_ptr<List>list;

	// プリミティブ
	std::weak_ptr<Primitive>prim;

	// ビルド入力情報
	std::unique_ptr<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS>input;

	// インスタンス数
	size_t instanceNum;

	// バッファ
	D3D12_RAYTRACING_INSTANCE_DESC* buf;

	// ボトムレベル
	std::vector<Acceleration*>bottom;
};
