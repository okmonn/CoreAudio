#include "../include/ShaderLoader.h"
#include "../include/Code.h"
#include <cassert>
#include <dxcapi.h>
#include <d3dcompiler.h>

ShaderLoader::ShaderLoader(const std::string& file_path, const std::string& entry_func_name, const std::string& shader_model)
{
    ShaderCompile(file_path, entry_func_name, shader_model);
}

void ShaderLoader::ShaderCompile(const std::string& file_path, const std::string& entry_func_name, const std::string& shader_model)
{
    okmonn::Code file(file_path);
    okmonn::Code entry(entry_func_name);
    okmonn::Code model(shader_model);

    auto hr = D3DCompileFromFile(file.UniCode().c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry.MultibyteCode().c_str(), model.MultibyteCode().c_str(),
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0U, (ID3DBlob**)GetAddress(), nullptr);
    assert(hr == S_OK);
    return;

    /*DXIL‚Ì‚æ‚éƒRƒ“ƒpƒCƒ‹*/
    ComPtr<IDxcLibrary>library;
    hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
    assert(hr == S_OK);

    ComPtr<IDxcIncludeHandler>handler;
    hr = library->CreateIncludeHandler(&handler);
    assert(hr == S_OK);

    ComPtr<IDxcBlobEncoding>encode;
    hr = library->CreateBlobFromFile(file.UniCode().c_str(), nullptr, &encode);
    assert(hr == S_OK);

    ComPtr<IDxcCompiler>compler;
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compler));
    assert(hr == S_OK);

    ComPtr<IDxcOperationResult>result;
    hr = compler->Compile(encode.Get(), file.UniCode().c_str(),
        entry.UniCode().c_str(), model.UniCode().c_str(), nullptr, 0U, nullptr, 0U, handler.Get(), &result);
    assert(hr == S_OK);

    result->GetStatus(&hr);
    assert(hr == S_OK);
    hr = result->GetResult((IDxcBlob**)GetAddress());
    assert(hr == S_OK);
}
