#pragma once
#include <string>
#include <memory>
#include <unordered_map>

struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct D3D12_STATE_SUBOBJECT;

class ShaderLibrary
{
public:
	// コンパイル
	void Compile(const std::string& fileName, std::string& entry, const std::initializer_list<std::string>& func, const std::string& ver);

	// サブオブジェクトの取得
	D3D12_STATE_SUBOBJECT Sub(const std::string& fileName);

	// インスタンス変数の取得
	static ShaderLibrary& Get(void);

private:
	ShaderLibrary(const ShaderLibrary&) = delete;
	void operator=(const ShaderLibrary&) = delete;

	// コンストラクタ
	ShaderLibrary();
	// デストラクタ
	~ShaderLibrary();

	// マルチバイト文字をユニコード文字に変換
	std::wstring ChangeCode(const std::string& str);

	// コンパイル
	void Compile(const std::string& fileName, const std::string& entry, const std::string& ver, ID3DBlob** blob);


	// サブオブジェクト
	std::unordered_map<std::string, std::unique_ptr<D3D12_STATE_SUBOBJECT>>sub;
};
