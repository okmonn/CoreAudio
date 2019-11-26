#include "ShaderLibrary.h"
#include <wrl.h>
#include <vector>
#include <d3d12.h>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

// コンストラクタ
ShaderLibrary::ShaderLibrary()
{
}

// デストラクタ
ShaderLibrary::~ShaderLibrary()
{
}

// マルチバイト文字をユニコード文字に変換
std::wstring ShaderLibrary::ChangeCode(const std::string& str)
{
	std::vector<wchar_t>buf(MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &buf.front(), int(buf.size()));

	return std::wstring(buf.begin(), buf.end());
}

// コンパイル
void ShaderLibrary::Compile(const std::string& fileName, const std::string& entry, const std::string& ver, ID3DBlob** blob)
{
	std::wstring name   = ChangeCode(fileName);
	std::wstring func   = ChangeCode(entry);
	std::wstring target = ChangeCode(ver);

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
		hr = result->GetResult((IDxcBlob**)&(*blob));
		_ASSERT(hr == S_OK);
	}
	else
	{
		Microsoft::WRL::ComPtr<IDxcBlobEncoding>print   = nullptr;
		Microsoft::WRL::ComPtr<IDxcBlobEncoding>print16 = nullptr;

		hr = result->GetErrorBuffer(&print);
		_ASSERT(hr == S_OK);

		hr = library->GetBlobAsUtf16(print.Get(), &print16);
		_ASSERT(hr == S_OK);

		wprintf(L"%*s", (int)print16->GetBufferSize() / 2, (LPCWSTR)print16->GetBufferPointer());
	}
}

// コンパイル
void ShaderLibrary::Compile(const std::string& fileName, std::string& entry, const std::initializer_list<std::string>& func, const std::string& ver)
{
	if (sub.find(fileName) == sub.end())
	{
		Microsoft::WRL::ComPtr<ID3DBlob>blob = nullptr;
		Compile(fileName, entry, ver, &blob);

		std::vector<D3D12_EXPORT_DESC>expo(func.size());

		auto itr = func.begin();
		for (auto& i : expo)
		{
			i.ExportToRename = nullptr;
			i.Flags          = D3D12_EXPORT_FLAGS::D3D12_EXPORT_FLAG_NONE;
			i.Name           = ChangeCode(*itr++).c_str();
		}

		D3D12_DXIL_LIBRARY_DESC desc{};
		desc.DXILLibrary.BytecodeLength  = blob->GetBufferSize();
		desc.DXILLibrary.pShaderBytecode = blob->GetBufferPointer();
		desc.NumExports                  = unsigned int(func.size());
		desc.pExports                    = expo.data();

		sub[fileName]        = std::make_unique<D3D12_STATE_SUBOBJECT>();
		sub[fileName]->pDesc = &desc;
		sub[fileName]->Type  = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	}
}

// サブオブジェクトの取得
D3D12_STATE_SUBOBJECT ShaderLibrary::Sub(const std::string& fileName)
{
	if (sub.find(fileName) != sub.end())
	{
		return *sub[fileName];
	}

	return D3D12_STATE_SUBOBJECT();
}

// インスタンス変数の取得
ShaderLibrary& ShaderLibrary::Get(void)
{
	static ShaderLibrary instance;
	return instance;
}
