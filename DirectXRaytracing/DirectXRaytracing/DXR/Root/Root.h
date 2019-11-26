#pragma once
#include <string>
#include <memory>

struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct D3D12_STATE_SUBOBJECT;
struct ID3D12RootSignature;

class Root
{
public:
	// コンストラクタ
	Root();
	Root(const std::string& fileName, const std::string& entry, const std::initializer_list<std::string>& func, const std::string& ver);
	// デストラクタ
	~Root();

	// ルートシグネチャの取得
	ID3D12RootSignature* Signature(void) const;

	//	シェーダのサブオブジェクトの取得
	D3D12_STATE_SUBOBJECT Shader(void) const;

	// ルートシグネチャのサブオブジェクトの取得
	D3D12_STATE_SUBOBJECT Sub(void) const;

private:
	// ルートシグネチャの生成
	void CreateRoot(void);

	// ルートシグネチャの生成
	void CreateRoot(ID3DBlob* blob);

	// ローカルルートシグネチャの生成
	void CreateLocal(void);

	// グローバルルートシグネチャの生成
	void CreateGlobal(void);


	//	シェーダのサブオブジェクト
	std::unique_ptr<D3D12_STATE_SUBOBJECT>shader;

	// ルートシグネチャのサブオブジェクト
	std::unique_ptr<D3D12_STATE_SUBOBJECT>sub;

	// ルートシグネチャ
	ID3D12RootSignature* root;
};
