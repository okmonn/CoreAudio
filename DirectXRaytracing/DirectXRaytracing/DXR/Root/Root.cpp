#include "Root.h"
#include "../Device/Device.h"
#include "../Helper.h"
#include <wrl.h>
#include <vector>
#include <d3d12.h>
#include <dxcapi.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "d3dcompiler.lib")

// シェーダのコンパイル
void Compile(const std::string& fileName, const std::string& entry, const std::string& ver, ID3DBlob** blob)
{
	std::wstring name   = DXR::ChangeCode(fileName);
	std::wstring func   = DXR::ChangeCode(entry);
	std::wstring target = DXR::ChangeCode(ver);

	Microsoft::WRL::ComPtr<IDxcLibrary>library = nullptr;
	auto hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IDxcIncludeHandler>handler = nullptr;
	library->CreateIncludeHandler(&handler);
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IDxcBlobEncoding>encode = nullptr;
	library->CreateBlobFromFile(name.c_str(), nullptr, &encode);
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IDxcCompiler>compiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IDxcOperationResult>result = nullptr;
	hr = compiler->Compile(encode.Get(), name.c_str(), func.c_str(), target.c_str(), nullptr, 0, nullptr, 0, handler.Get(), &result);
	_ASSERT(hr == S_OK);

	result->GetStatus(&hr);
	if (SUCCEEDED(hr))
	{
		hr = result->GetResult((IDxcBlob**) & (*blob));
		_ASSERT(hr == S_OK);
	}
	else
	{
		Microsoft::WRL::ComPtr<IDxcBlobEncoding>print = nullptr;
		Microsoft::WRL::ComPtr<IDxcBlobEncoding>print16 = nullptr;

		hr = result->GetErrorBuffer(&print);
		_ASSERT(hr == S_OK);

		hr = library->GetBlobAsUtf16(print.Get(), &print16);
		_ASSERT(hr == S_OK);

		wprintf(L"%*s", (int)print16->GetBufferSize() / 2, (LPCWSTR)print16->GetBufferPointer());
	}
}

// シェーダとの関連付け
void ShaderAssociation(D3D12_STATE_SUBOBJECT& sub, const std::initializer_list<std::string>& func, ID3DBlob* blob)
{
	std::vector<D3D12_EXPORT_DESC>expo(func.size());
	auto itr = func.begin();
	for (auto& i : expo)
	{
		i.ExportToRename = nullptr;
		i.Flags = D3D12_EXPORT_FLAGS::D3D12_EXPORT_FLAG_NONE;
		i.Name  = DXR::ChangeCode(*itr++).c_str();
	}

	D3D12_DXIL_LIBRARY_DESC desc{};
	desc.DXILLibrary.BytecodeLength = blob->GetBufferSize();
	desc.DXILLibrary.pShaderBytecode = blob->GetBufferPointer();
	desc.NumExports = unsigned int(func.size());
	desc.pExports = expo.data();

	sub.pDesc = &desc;
	sub.Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
}

// コンストラクタ
Root::Root() :
	shader(nullptr), sub(std::make_unique<D3D12_STATE_SUBOBJECT>()), root(nullptr)
{
	CreateRoot();
	CreateGlobal();
}

// コンストラクタ
Root::Root(const std::string& fileName, const std::string& entry, const std::initializer_list<std::string>& func, const std::string& ver) : 
	shader(std::make_unique<D3D12_STATE_SUBOBJECT>()), sub(std::make_unique<D3D12_STATE_SUBOBJECT>()), root(nullptr)
{
	Microsoft::WRL::ComPtr<ID3DBlob>blob = nullptr;
	Compile(fileName, entry, ver, &blob);
	ShaderAssociation(*shader, func, blob.Get());
	CreateRoot(blob.Get());
	CreateLocal();
}

// デストラクタ
Root::~Root()
{
	if (root != nullptr)
	{
		root->Release();
		root = nullptr;
	}
}

// ルートシグネチャの生成
void Root::CreateRoot(void)
{
	Microsoft::WRL::ComPtr<ID3DBlob>sig = nullptr;
	D3D12_ROOT_SIGNATURE_DESC desc{};
	auto hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, &sig, nullptr);
	_ASSERT(hr == S_OK);

	hr = Device::Get().Dev()->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&root));
	_ASSERT(hr == S_OK);
}

// ルートシグネチャの生成
void Root::CreateRoot(ID3DBlob* blob)
{
	Microsoft::WRL::ComPtr<ID3DBlob>sig = nullptr;
	auto hr = D3DGetBlobPart(blob->GetBufferPointer(), blob->GetBufferSize(), D3D_BLOB_PART::D3D_BLOB_ROOT_SIGNATURE, 0, &sig);
	_ASSERT(hr == S_OK);

	hr = Device::Get().Dev()->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&root));
	_ASSERT(hr == S_OK);
}

// グローバルルートシグネチャの生成
void Root::CreateGlobal(void)
{
	sub->pDesc = &root;
	sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
}

// ローカルルートシグネチャの生成
void Root::CreateLocal(void)
{
	sub->pDesc = &root;
	sub->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
}

// ルートシグネチャの取得
ID3D12RootSignature* Root::Signature(void) const
{
	return root;
}

//	シェーダのサブオブジェクトの取得
D3D12_STATE_SUBOBJECT Root::Shader(void) const
{
	return *shader;
}

// ルートシグネチャのサブオブジェクトの取得
D3D12_STATE_SUBOBJECT Root::Sub(void) const
{
	return *sub;
}
