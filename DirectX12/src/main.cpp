#include "../include/Window.h"
#include "../include/Device.h"
#include "../include/CommandAllocator.h"
#include "../include/CommandList.h"
#include "../include/CommandQueue.h"
#include "../include/Fence.h"
#include "../include/SwapChain.h"
#include "../include/RenderTarget.h"
#include "../include/DepthStencil.h"
#include "../include/ShaderLoader.h"
#include "../include/RootSignature.h"
#include "../include/Pipeline.h"
#include "../include/VertexBuffer.h"
#include "../include/IndexBuffer.h"
#include "../include/ConstantBuffer.h"
#include "../include/ResourceBuffer.h"
#include "Function.h"
#include "Bitmap.h"
#include "Pmd.h"
#include "Vmd.h"
#include <Windows.h>
#include <algorithm>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "OkmonnFunction.lib")

namespace {
    /*ルート情報*/
    struct RootInfo {
        /*範囲*/
        std::vector<D3D12_DESCRIPTOR_RANGE1>range;
        /*パラメータ*/
        std::vector<D3D12_ROOT_PARAMETER1>param;
        /*サンプラー*/
        std::vector<D3D12_STATIC_SAMPLER_DESC>sampler;
    };
    /*テクスチャ2D用頂点データ*/
    struct Texture2DVertex {
        /*座標*/
        okmonn::Vec3f pos;
        /*UV座標*/
        okmonn::Vec2f uv;
    };
}

namespace {
    /*ウィンドウサイズ*/
    const okmonn::Vec2 window_size(640, 480);
    /*クリア色*/
    const float clear_color[] = {
        1.0f, 0.0f, 0.0f, 1.0f
    };
    /*プリミティブ2D用頂点シェーダファイルパス*/
    const std::string primitive2D_vertex_file_path = "../../shader/PrimitiveVertex2D.hlsl";
    /*プリミティブ2D用ピクセルシェーダファイルパス*/
    const std::string primitive2D_pixel_file_path = "../../shader/PrimitivePixel2D.hlsl";
    /*テクスチャ2D用頂点シェーダファイルパス*/
    const std::string texture2D_vertex_file_path = "../../shader/TextureVertex2D.hlsl";
    /*テクスチャ2D用ピクセルシェーダファイルパス*/
    const std::string texture2D_pixel_file_path = "../../shader/TexturePixel2D.hlsl";
    /*プリミティブ3D用頂点シェーダファイルパス*/
    const std::string primitive3D_vertex_file_path = "../../shader/PrimitiveVertex3D.hlsl";
    /*プリミティブ3D用ピクセルシェーダファイルパス*/
    const std::string primitive3D_pixel_file_path = "../../shader/PrimitivePixel3D.hlsl";
    /*テクスチャ3D用頂点シェーダファイルパス*/
    const std::string texture3D_vertex_file_path = "../../shader/TextureVertex3D.hlsl";
    /*テクスチャ3D用ピクセルシェーダファイルパス*/
    const std::string texture3D_pixel_file_path = "../../shader/TexturePixel3D.hlsl";
    /*モデル用頂点シェーダファイルパス*/
    const std::string model_vertex_file_path = "../../shader/ModelVertex.hlsl";
    /*モデル用ピクセルシェーダファイルパス*/
    const std::string model_pixel_file_path = "../../shader/ModelPixel.hlsl";
    /*シェーダエントリー関数名*/
    const std::string entry_func_name = "main";
    /*頂点シェーダモデル*/
    const std::string vertex_shader_model = "vs_5_1";
    /*ピクセルシェーダモデル*/
    const std::string pixel_shader_model = "ps_5_1";
    /*入力属性*/
    const D3D12_INPUT_ELEMENT_DESC input[] = {
        { "POSITION", 0,
        DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,
        DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0,
        DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BONENO", 0,
        DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "WEIGHT", 0,
        DXGI_FORMAT::DXGI_FORMAT_R8_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    /*プリミティブ2D用頂点データ*/
    const okmonn::Vec3f primitive2D_vertex[] = {
        okmonn::Vec3f(0.0f, 0.0f, 0.0f),                     okmonn::Vec3f(float(window_size.x / 2), 0.0f, 0.0f),
        okmonn::Vec3f(0.0f, float(window_size.y / 2), 0.0f), okmonn::Vec3f(float(window_size.x / 2), float(window_size.y / 2), 0.0f)
    };
    /*プリミティブ用インデックスデータ*/
    const std::uint16_t primitive_index[] = {
        0, 1, 2, 1, 2, 3
    };
    /*プリミティブ2D用定数データ*/
    const struct Primitive2DConstant {
        /*色*/
        okmonn::Vec4f color;
        /*行列*/
        okmonn::Matrix3x4 mtx;
    } primitive2D_constant;
    /*テクスチャ2D用頂点データ*/
    const Texture2DVertex texture2D_vertex[] = {
        { okmonn::Vec3f(0.0f, 0.0f, 0.0f),                                         okmonn::Vec2f(0.0f, 0.0f) },
        { okmonn::Vec3f(float(window_size.x / 2), 0.0f, 0.0f),                     okmonn::Vec2f(1.0f, 0.0f) },
        { okmonn::Vec3f(0.0f, float(window_size.y / 2), 0.0f),                     okmonn::Vec2f(0.0f, 1.0f) },
        { okmonn::Vec3f(float(window_size.x / 2), float(window_size.y / 2), 0.0f), okmonn::Vec2f(1.0f, 1.0f) }
    };
    /*テクスチャ用インデックスデータ*/
    const std::uint16_t texture_index[] = {
        0, 1, 2, 1, 2, 3
    };
    /*プリミティブ3D用頂点データ*/
    const okmonn::Vec3f primitive3D_vertex[] = {
        okmonn::Vec3f(-10.0f,  10.0f, 0.0f), okmonn::Vec3f(10.0f,  10.0f, 0.0f),
        okmonn::Vec3f(-10.0f, -10.0f, 0.0f), okmonn::Vec3f(10.0f, -10.0f, 0.0f)
    };
    /*プリミティブ3D用定数データ*/
    struct Primitive3DConstant {
        /*色*/
        okmonn::Vec4f color;
        /*ワールド行列*/
        okmonn::Matrix3x4 world;
        /*ビュー行列*/
        okmonn::Matrix4x4 view;
        /*パースペクティブ行列*/
        okmonn::Matrix4x4 perspective;
    }primitive3D_constant;
    /*テクスチャ3D用頂点データ*/
    const Texture2DVertex texture3D_vertex[] = {
        { okmonn::Vec3f(-10.0f,  10.0f, 0.0f), okmonn::Vec2f(0.0f, 0.0f) },
        { okmonn::Vec3f( 10.0f,  10.0f, 0.0f), okmonn::Vec2f(1.0f, 0.0f) },
        { okmonn::Vec3f(-10.0f, -10.0f, 0.0f), okmonn::Vec2f(0.0f, 1.0f) },
        { okmonn::Vec3f( 10.0f, -10.0f, 0.0f), okmonn::Vec2f(1.0f, 1.0f) }
    };
    /*テクスチャ3D用定数データ*/
    struct Texture3DConstant {
        /*ワールド行列*/
        okmonn::Matrix3x4 world;
        /*ビュー行列*/
        okmonn::Matrix4x4 view;
        /*パースペクティブ行列*/
        okmonn::Matrix4x4 perspective;
    };
    /*モデル用定数データ*/
    struct ModelConstant {
        /*ワールド行列*/
        okmonn::Matrix3x4 world;
        /*ビュー行列*/
        okmonn::Matrix4x4 view;
        /*パースペクティブ行列*/
        okmonn::Matrix4x4 perspective;
    };
}

/*ウィンドウメッセージの確認
return 不可フラグ*/
static bool CheckMsg(void) {
    static MSG msg{};
    static const wchar_t* name = nullptr;
    static void* instance      = nullptr;

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        switch (msg.message)
        {
        case WM_CREATE:
        {
            LPWNDCLASSEX wnd = (LPWNDCLASSEX)GetWindowLongPtr(msg.hwnd, GWLP_HINSTANCE);
            name = wnd->lpszClassName;
            instance = wnd->hInstance;
            break;
        }
        case WM_QUIT:
            UnregisterClass(name, HINSTANCE(instance));
            return false;
        default:
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

/*プリミティブ2D用ルート情報の取得
return ルート情報*/
static RootInfo GetPrimitive2DRootInfo(void) {
    RootInfo info{};
    info.range.resize(1);
    info.param.resize(1);
    
    info.range[0].BaseShaderRegister                = 0;
    info.range[0].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    info.range[0].NumDescriptors                    = 1;
    info.range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    info.range[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    info.range[0].RegisterSpace                     = 0;

    info.param[0].DescriptorTable.NumDescriptorRanges = std::uint32_t(info.range.size());
    info.param[0].DescriptorTable.pDescriptorRanges   = info.range.data();
    info.param[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    info.param[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    return info;
}

/*テクスチャ2D用ルート情報の取得
return ルート情報*/
static RootInfo GetTexture2DRootInfo(void) {
    RootInfo info{};
    info.range.resize(2);
    info.param.resize(2);
    info.sampler.resize(1);

    info.range[0].BaseShaderRegister                = 0;
    info.range[0].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    info.range[0].NumDescriptors                    = 1;
    info.range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    info.range[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    info.range[0].RegisterSpace                     = 0;
    info.range[1].BaseShaderRegister                = 0;
    info.range[1].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    info.range[1].NumDescriptors                    = 1;
    info.range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    info.range[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    info.range[1].RegisterSpace                     = 0;

    info.param[0].DescriptorTable.NumDescriptorRanges = 1;
    info.param[0].DescriptorTable.pDescriptorRanges   = &info.range[0];
    info.param[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    info.param[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
    info.param[1].DescriptorTable.NumDescriptorRanges = 1;
    info.param[1].DescriptorTable.pDescriptorRanges   = &info.range[1];
    info.param[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    info.param[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    info.sampler[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].BorderColor      = D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    info.sampler[0].ComparisonFunc   = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
    info.sampler[0].Filter           = D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    info.sampler[0].MaxAnisotropy    = 0;
    info.sampler[0].MaxLOD           = float(0xFFFFFFFF);
    info.sampler[0].MinLOD           = 0.0f;
    info.sampler[0].MipLODBias       = 0.0f;
    info.sampler[0].RegisterSpace    = 0;
    info.sampler[0].ShaderRegister   = 0;
    info.sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
    
    return info;
}

/*プリミティブ3D用ルート情報の取得
return ルート情報*/
static RootInfo GetPrimitive3DRootInfo(void) {
    RootInfo info{};
    info.range.resize(1);
    info.param.resize(1);

    info.range[0].BaseShaderRegister                = 0;
    info.range[0].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    info.range[0].NumDescriptors                    = 1;
    info.range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    info.range[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    info.range[0].RegisterSpace                     = 0;

    info.param[0].DescriptorTable.NumDescriptorRanges = std::uint32_t(info.range.size());
    info.param[0].DescriptorTable.pDescriptorRanges   = info.range.data();
    info.param[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    info.param[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    return info;
}

/*テクスチャ3D用ルート情報の取得
return ルート情報*/
static RootInfo GetTexture3DRootInfo(void) {
    RootInfo info{};
    info.range.resize(2);
    info.param.resize(2);
    info.sampler.resize(1);

    info.range[0].BaseShaderRegister                = 0;
    info.range[0].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    info.range[0].NumDescriptors                    = 1;
    info.range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    info.range[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    info.range[0].RegisterSpace                     = 0;
    info.range[1].BaseShaderRegister                = 0;
    info.range[1].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    info.range[1].NumDescriptors                    = 1;
    info.range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    info.range[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    info.range[1].RegisterSpace                     = 0;

    info.param[0].DescriptorTable.NumDescriptorRanges = 1;
    info.param[0].DescriptorTable.pDescriptorRanges   = &info.range[0];
    info.param[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    info.param[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
    info.param[1].DescriptorTable.NumDescriptorRanges = 1;
    info.param[1].DescriptorTable.pDescriptorRanges   = &info.range[1];
    info.param[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    info.param[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    info.sampler[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].BorderColor      = D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    info.sampler[0].ComparisonFunc   = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
    info.sampler[0].Filter           = D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    info.sampler[0].MaxAnisotropy    = 0;
    info.sampler[0].MaxLOD           = float(0xFFFFFFFF);
    info.sampler[0].MinLOD           = 0.0f;
    info.sampler[0].MipLODBias       = 0.0f;
    info.sampler[0].RegisterSpace    = 0;
    info.sampler[0].ShaderRegister   = 0;
    info.sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    return info;
}

/*モデル用ルート情報の取得
return モデル情報*/
static RootInfo GetModelRootInfo(void) {
    const std::uint32_t constant_num = 3;
    const std::uint32_t resource_num = 1;

    RootInfo info{};
    info.range.resize(constant_num + resource_num);
    info.param.resize(info.range.size());
    info.sampler.resize(1);

    for (std::uint32_t i = 0; i < constant_num; ++i) {
        info.range[i].BaseShaderRegister                = i;
        info.range[i].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        info.range[i].NumDescriptors                    = 1;
        info.range[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        info.range[i].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        info.range[i].RegisterSpace                     = 0;

        info.param[i].DescriptorTable.NumDescriptorRanges = 1;
        info.param[i].DescriptorTable.pDescriptorRanges   = &info.range[i];
        info.param[i].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        info.param[i].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
    }

    for (std::uint32_t i = 0; i < resource_num; ++i) {
        info.range[constant_num + i].BaseShaderRegister                = i;
        info.range[constant_num + i].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        info.range[constant_num + i].NumDescriptors                    = 1;
        info.range[constant_num + i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        info.range[constant_num + i].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        info.range[constant_num + i].RegisterSpace                     = 0;

        info.param[constant_num + i].DescriptorTable.NumDescriptorRanges = 1;
        info.param[constant_num + i].DescriptorTable.pDescriptorRanges   = &info.range[constant_num + i];
        info.param[constant_num + i].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        info.param[constant_num + i].ShaderVisibility                    = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
    }

    info.sampler[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    info.sampler[0].BorderColor      = D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    info.sampler[0].ComparisonFunc   = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
    info.sampler[0].Filter           = D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    info.sampler[0].MaxAnisotropy    = 0;
    info.sampler[0].MaxLOD           = float(0xFFFFFFFF);
    info.sampler[0].MinLOD           = 0.0f;
    info.sampler[0].MipLODBias       = 0.0f;
    info.sampler[0].RegisterSpace    = 0;
    info.sampler[0].ShaderRegister   = 0;
    info.sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    return info;
}

#include <DirectXMath.h>
using namespace DirectX;

/*ボーン行列の再帰処理
.モデルボーン情報
.行列配列
.適用行列*/
static void RecursionBoneMatrix(okmonn::ModelBorn& bone, std::vector<okmonn::Matrix4x4>& bone_matrix, const okmonn::Matrix4x4& matrix)
{
    bone_matrix[bone.bone_index] = bone_matrix[bone.bone_index] * matrix;

    for (auto& i : bone.children) {
        RecursionBoneMatrix(*i, bone_matrix, bone_matrix[bone.bone_index]);
    }
}

/*ボーン回転*/
static void RotateBone(okmonn::ModelBorn& bone, std::vector<okmonn::Matrix4x4>& bone_matrix, const okmonn::Vec4f& rotation)
{
    okmonn::Matrix4x4 q(
        1.0f - (2.0f * std::pow(rotation.y, 2)) - (2.0f * std::pow(rotation.z, 2)),
        (2.0f * rotation.x * rotation.y) - (2.0f * rotation.w * rotation.z),
        (2.0f * rotation.x * rotation.z) + (2.0f * rotation.w * rotation.y),
        0.0f,
        
        (2.0f * rotation.x * rotation.y) + (2.0f * rotation.w * rotation.z),
        1.0f - (2.0f * std::pow(rotation.x, 2)) - (2.0f * std::pow(rotation.z, 2)),
        (2.0f * rotation.y * rotation.z) - (2.0f * rotation.w * rotation.x),
        0.0f,
        
        (2.0f * rotation.x * rotation.z) - (2.0f * rotation.w * rotation.y),
        (2.0f * rotation.y * rotation.z) + (2.0f * rotation.w * rotation.x),
        1.0f - (2.0f * std::pow(rotation.x, 2)) - (2.0f * std::pow(rotation.y, 2)),
        0.0f,
        
        0.0f, 0.0f, 0.0f, 1.0f);

    okmonn::Matrix4x4 tmp1;
    tmp1.Translation(-bone.head_pos);
    okmonn::Matrix4x4 tmp2;
    tmp2.Translation(bone.head_pos);

    bone_matrix[bone.bone_index] = tmp1 * q * tmp2;
}

static void RotateBone(okmonn::ModelBorn& bone, std::vector<okmonn::Matrix4x4>& bone_matrix, const okmonn::Vec4f& rotation1, const okmonn::Vec4f& rotation2, const float& rate)
{
    /*okmonn::Matrix4x4 q1(
        1.0f - (2.0f * std::pow(rotation1.y, 2)) - (2.0f * std::pow(rotation1.z, 2)),
        (2.0f * rotation1.x * rotation1.y) - (2.0f * rotation1.w * rotation1.z),
        (2.0f * rotation1.x * rotation1.z) + (2.0f * rotation1.w * rotation1.y),
        0.0f,

        (2.0f * rotation1.x * rotation1.y) + (2.0f * rotation1.w * rotation1.z),
        1.0f - (2.0f * std::pow(rotation1.x, 2)) - (2.0f * std::pow(rotation1.z, 2)),
        (2.0f * rotation1.y * rotation1.z) - (2.0f * rotation1.w * rotation1.x),
        0.0f,

        (2.0f * rotation1.x * rotation1.z) - (2.0f * rotation1.w * rotation1.y),
        (2.0f * rotation1.y * rotation1.z) + (2.0f * rotation1.w * rotation1.x),
        1.0f - (2.0f * std::pow(rotation1.x, 2)) - (2.0f * std::pow(rotation1.y, 2)),
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f);
    for (std::uint32_t y = 0; y < 4; ++y) {
        for (std::uint32_t x = 0; x < 4; ++x) {
            q1.m[y][x] *= (1.0f - rate);
        }
    }

    okmonn::Matrix4x4 q2(
        1.0f - (2.0f * std::pow(rotation2.y, 2)) - (2.0f * std::pow(rotation2.z, 2)),
        (2.0f * rotation2.x * rotation2.y) - (2.0f * rotation2.w * rotation2.z),
        (2.0f * rotation2.x * rotation2.z) + (2.0f * rotation2.w * rotation2.y),
        0.0f,

        (2.0f * rotation2.x * rotation2.y) + (2.0f * rotation2.w * rotation2.z),
        1.0f - (2.0f * std::pow(rotation2.x, 2)) - (2.0f * std::pow(rotation2.z, 2)),
        (2.0f * rotation2.y * rotation2.z) - (2.0f * rotation2.w * rotation2.x),
        0.0f,

        (2.0f * rotation2.x * rotation2.z) - (2.0f * rotation2.w * rotation2.y),
        (2.0f * rotation2.y * rotation2.z) + (2.0f * rotation2.w * rotation2.x),
        1.0f - (2.0f * std::pow(rotation2.x, 2)) - (2.0f * std::pow(rotation2.y, 2)),
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f);
    for (std::uint32_t y = 0; y < 4; ++y) {
        for (std::uint32_t x = 0; x < 4; ++x) {
            q2.m[y][x] *= rate;
        }
    }

    okmonn::Matrix4x4 q;
    for (std::uint32_t y = 0; y < 4; ++y) {
        for (std::uint32_t x = 0; x < 4; ++x) {
            q.m[y][x] = q1.m[y][x] + q2.m[y][x];
        }
    }*/

    okmonn::Matrix4x4 q;
    /*if (rate <= 0.0f) {
        q = okmonn::Matrix4x4(
            1.0f - (2.0f * std::pow(rotation1.y, 2)) - (2.0f * std::pow(rotation1.z, 2)),
            (2.0f * rotation1.x * rotation1.y) - (2.0f * rotation1.w * rotation1.z),
            (2.0f * rotation1.x * rotation1.z) + (2.0f * rotation1.w * rotation1.y),
            0.0f,

            (2.0f * rotation1.x * rotation1.y) + (2.0f * rotation1.w * rotation1.z),
            1.0f - (2.0f * std::pow(rotation1.x, 2)) - (2.0f * std::pow(rotation1.z, 2)),
            (2.0f * rotation1.y * rotation1.z) - (2.0f * rotation1.w * rotation1.x),
            0.0f,

            (2.0f * rotation1.x * rotation1.z) - (2.0f * rotation1.w * rotation1.y),
            (2.0f * rotation1.y * rotation1.z) + (2.0f * rotation1.w * rotation1.x),
            1.0f - (2.0f * std::pow(rotation1.x, 2)) - (2.0f * std::pow(rotation1.y, 2)),
            0.0f,

            0.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (rate >= 1.0f) {
        q = okmonn::Matrix4x4(
            1.0f - (2.0f * std::pow(rotation2.y, 2)) - (2.0f * std::pow(rotation2.z, 2)),
            (2.0f * rotation2.x * rotation2.y) - (2.0f * rotation2.w * rotation2.z),
            (2.0f * rotation2.x * rotation2.z) + (2.0f * rotation2.w * rotation2.y),
            0.0f,

            (2.0f * rotation2.x * rotation2.y) + (2.0f * rotation2.w * rotation2.z),
            1.0f - (2.0f * std::pow(rotation2.x, 2)) - (2.0f * std::pow(rotation2.z, 2)),
            (2.0f * rotation2.y * rotation2.z) - (2.0f * rotation2.w * rotation2.x),
            0.0f,

            (2.0f * rotation2.x * rotation2.z) - (2.0f * rotation2.w * rotation2.y),
            (2.0f * rotation2.y * rotation2.z) + (2.0f * rotation2.w * rotation2.x),
            1.0f - (2.0f * std::pow(rotation2.x, 2)) - (2.0f * std::pow(rotation2.y, 2)),
            0.0f,

            0.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
        
    }*/

    float angle = std::acos((rotation1.x * rotation2.x) + (rotation1.y * rotation2.y)
        + (rotation1.z * rotation2.z) + (rotation1.w * rotation2.w));
    float p1 = std::sin(angle * (1.0f - rate));
    float p2 = std::sin(angle * rate);
    okmonn::Vec4f p = ((rotation1 * p1) + (rotation2 * p2)) / std::sin(angle);

    q = okmonn::Matrix4x4(
        1.0f - (2.0f * std::pow(p.y, 2)) - (2.0f * std::pow(p.z, 2)),
        (2.0f * p.x * p.y) - (2.0f * p.w * p.z),
        (2.0f * p.x * p.z) + (2.0f * p.w * p.y),
        0.0f,

        (2.0f * p.x * p.y) + (2.0f * p.w * p.z),
        1.0f - (2.0f * std::pow(p.x, 2)) - (2.0f * std::pow(p.z, 2)),
        (2.0f * p.y * p.z) - (2.0f * p.w * p.x),
        0.0f,

        (2.0f * p.x * p.z) - (2.0f * p.w * p.y),
        (2.0f * p.y * p.z) + (2.0f * p.w * p.x),
        1.0f - (2.0f * std::pow(p.x, 2)) - (2.0f * std::pow(p.y, 2)),
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f);

    okmonn::Matrix4x4 tmp1;
    tmp1.Translation(-bone.head_pos);
    okmonn::Matrix4x4 tmp2;
    tmp2.Translation(bone.head_pos);

    bone_matrix[bone.bone_index] = tmp1 * q * tmp2;
}

int main() {
    Window window(window_size);
    Device device;
    CommandAllocator command_allocator(device);
    CommandList command_list(device);
    CommandQueue command_queue(device);
    Fence fence(device, command_queue);
    SwapChain swap_chain(window, command_queue);
    RenderTarget render_target(device, swap_chain);
    DepthStencil depth_stencil(device, window_size);

    ShaderLoader primitive2D_shader_vertex(primitive2D_vertex_file_path, entry_func_name, vertex_shader_model);
    ShaderLoader primitive2D_shader_pixel(primitive2D_pixel_file_path, entry_func_name, pixel_shader_model);
    auto primitive2D_root_info = GetPrimitive2DRootInfo();
    RootSignature primitive2D_root_signature(device, primitive2D_root_info.param, primitive2D_root_info.sampler);
    Pipeline primitive2D_pipeline(device, swap_chain, primitive2D_root_signature, { primitive2D_shader_vertex.Get(), primitive2D_shader_pixel.Get() },
        { input[0] }, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    VertexBuffer primitive2D_vertex_buffer(device, sizeof(primitive2D_vertex));
    primitive2D_vertex_buffer.UpDateVertex(primitive2D_vertex);
    IndexBuffer primitive2D_index_buffer(device, sizeof(primitive_index));
    primitive2D_index_buffer.UpDateIndex(primitive_index);
    Primitive2DConstant primitive2D_constant{};
    primitive2D_constant.color = 1.0f;
    primitive2D_constant.mtx = okmonn::Matrix3x4(okmonn::Vec3f((2.0f / float(window_size.x)), 0.0f, 0.0f),
                                                 okmonn::Vec3f( 0.0f, -(2.0f / float(window_size.y)), 0.0f),
                                                 okmonn::Vec3f( 0.0f, 0.0f, 1.0f),
                                                 okmonn::Vec3f( 0.0f, 0.0f, 0.0f))
                             * okmonn::Matrix3x4(okmonn::Vec3f( 1.0f, 0.0f, 0.0f),
                                                 okmonn::Vec3f( 0.0f, 1.0f, 0.0f),
                                                 okmonn::Vec3f( 0.0f, 0.0f, 1.0f),
                                                 okmonn::Vec3f(-1.0f, 1.0f, 0.0f));
    ConstantBuffer primitive2D_constant_buffer(device, sizeof(primitive2D_constant));
    primitive2D_constant_buffer.UpDateConstant(&primitive2D_constant, sizeof(primitive2D_constant));

    std::vector<std::uint8_t>bmp;
    std::uint32_t bmp_width  = 0;
    std::uint32_t bmp_height = 0;
    okmonn::LoadBitmapFromFile("../../rsc/MyLogo.bmp", bmp, &bmp_width, &bmp_height);
    ShaderLoader texture2D_shader_vertex(texture2D_vertex_file_path, entry_func_name, vertex_shader_model);
    ShaderLoader texture2D_shader_pixel(texture2D_pixel_file_path, entry_func_name, pixel_shader_model);
    auto texture2D_root_info = GetTexture2DRootInfo();
    RootSignature texture2D_root_signature(device, texture2D_root_info.param, texture2D_root_info.sampler);
    Pipeline texture2D_pipeline(device, swap_chain, texture2D_root_signature, { texture2D_shader_vertex.Get(), texture2D_shader_pixel.Get() },
        { input[0], input[1] }, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    VertexBuffer texture2D_vertex_buffer(device, sizeof(texture2D_vertex));
    texture2D_vertex_buffer.UpDateVertex(texture2D_vertex);
    IndexBuffer texture2D_index_buffer(device, sizeof(texture_index));
    texture2D_index_buffer.UpDateIndex(texture_index);
    okmonn::Matrix3x4 texture2D_constant{};
    texture2D_constant = okmonn::Matrix3x4(okmonn::Vec3f((2.0f / float(window_size.x)), 0.0f, 0.0f),
                                           okmonn::Vec3f( 0.0f, -(2.0f / float(window_size.y)), 0.0f),
                                           okmonn::Vec3f( 0.0f, 0.0f, 1.0f),
                                           okmonn::Vec3f( 0.0f, 0.0f, 0.0f))
                       * okmonn::Matrix3x4(okmonn::Vec3f( 1.0f, 0.0f, 0.0f),
                                           okmonn::Vec3f( 0.0f, 1.0f, 0.0f),
                                           okmonn::Vec3f( 0.0f, 0.0f, 1.0f),
                                           okmonn::Vec3f(-1.0f, 1.0f, 0.0f));
    ConstantBuffer texture2D_constant_buffer(device, sizeof(texture2D_constant));
    texture2D_constant_buffer.UpDateConstant(&texture2D_constant, sizeof(texture2D_constant));
    ResourceBuffer texture2D_resource_buffer(device, bmp_width, bmp_height);
    texture2D_resource_buffer.UpDateResource(bmp.data());

    ShaderLoader primitive3D_shader_vertex(primitive3D_vertex_file_path, entry_func_name, vertex_shader_model);
    ShaderLoader primitive3D_shader_pixel(primitive3D_pixel_file_path, entry_func_name, pixel_shader_model);
    auto primitive3D_root_info = GetPrimitive3DRootInfo();
    RootSignature primitive3D_root_signature(device, primitive3D_root_info.param, primitive3D_root_info.sampler);
    Pipeline primitive3D_pipeline(device, swap_chain, primitive3D_root_signature, { primitive3D_shader_vertex.Get(), primitive3D_shader_pixel.Get() },
        { input[0] }, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, true);
    VertexBuffer primitive3D_vertex_buffer(device, sizeof(primitive3D_vertex));
    primitive3D_vertex_buffer.UpDateVertex(primitive3D_vertex);
    IndexBuffer primitive3D_index_buffer(device, sizeof(primitive_index));
    primitive3D_index_buffer.UpDateIndex(primitive_index);
    Primitive3DConstant primitive3D_constant{};
    primitive3D_constant.color       = 1.0f;
    primitive3D_constant.view        = okmonn::GetViewMatrix(okmonn::Vec3f(0.0f, 0.0f, -30.0f), 0.0f);
    primitive3D_constant.perspective = okmonn::GetPerspectiveMatrix(okmonn::ConvertRadian(90.0f), float(window_size.x) / float(window_size.y), 0.1f, 300.0f);
    ConstantBuffer primitive3D_constant_buffer(device, sizeof(primitive3D_constant));
    primitive3D_constant_buffer.UpDateConstant(&primitive3D_constant, sizeof(primitive3D_constant));

    ShaderLoader texture3D_shader_vertex(texture3D_vertex_file_path, entry_func_name, vertex_shader_model);
    ShaderLoader texture3D_shader_pixel(texture3D_pixel_file_path, entry_func_name, pixel_shader_model);
    auto texture3D_root_info = GetTexture3DRootInfo();
    RootSignature texture3D_root_signature(device, texture3D_root_info.param, texture3D_root_info.sampler);
    Pipeline texture3D_pipeline(device, swap_chain, texture3D_root_signature, { texture3D_shader_vertex.Get(), texture3D_shader_pixel.Get() },
        { input[0], input[1] }, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, true);
    VertexBuffer texture3D_vertex_buffer(device, sizeof(texture3D_vertex));
    texture3D_vertex_buffer.UpDateVertex(texture3D_vertex);
    IndexBuffer texture3D_index_buffer(device, sizeof(texture_index));
    texture3D_index_buffer.UpDateIndex(texture_index);
    Texture3DConstant texture3D_constant{};
    texture3D_constant.view        = okmonn::GetViewMatrix(okmonn::Vec3f(0.0f, 0.0f, -30.0f), 0.0f);
    texture3D_constant.perspective = okmonn::GetPerspectiveMatrix(okmonn::ConvertRadian(90.0f), float(window_size.x) / float(window_size.y), 0.1f, 300.0f);
    ConstantBuffer texture3D_constant_buffer(device, sizeof(texture3D_constant));
    texture3D_constant_buffer.UpDateConstant(&texture3D_constant, sizeof(texture3D_constant));
    ResourceBuffer texture3D_resource_buffer(device, bmp_width, bmp_height);
    texture3D_resource_buffer.UpDateResource(bmp.data());

    std::vector<okmonn::ModelVertex>pmd_vertex;
    std::vector<std::uint16_t>pmd_index;
    std::vector<okmonn::ModelMaterial>pmd_material;
    std::vector<std::uint32_t>pmd_material_index;
    std::vector<std::string>pmd_material_texture_path;
    std::unordered_map<std::string, okmonn::ModelBorn>pmd_born;
    okmonn::LoadPmdFromFile("../../rsc/モデル/初音ミク.pmd", pmd_vertex, pmd_index, pmd_material, pmd_material_index, pmd_material_texture_path, pmd_born);
    ShaderLoader model_shader_vertex(model_vertex_file_path, entry_func_name, vertex_shader_model);
    ShaderLoader model_shader_pixel(model_pixel_file_path, entry_func_name, pixel_shader_model);
    auto model_root_info = GetModelRootInfo();
    RootSignature model_root_signature(device, model_root_info.param, model_root_info.sampler);
    Pipeline model_pipeline(device, swap_chain, model_root_signature, { model_shader_vertex.Get(), model_shader_pixel.Get() },
        { input[0], input[1], input[2], input[3], input[4] }, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, true);
    VertexBuffer model_vertex_buffer(device, std::uint32_t(sizeof(pmd_vertex[0]) * pmd_vertex.size()));
    model_vertex_buffer.UpDateVertex(pmd_vertex.data());
    IndexBuffer model_index_buffer(device, std::uint32_t(sizeof(pmd_index[0]) * pmd_index.size()));
    model_index_buffer.UpDateIndex(pmd_index.data());
    ModelConstant model_matrix;
    model_matrix.view        = okmonn::GetViewMatrix(okmonn::Vec3f(0.0f, 10.0f, -20.0f), okmonn::Vec3f(0.0f, 10.0f, 0.0f));
    model_matrix.perspective = okmonn::GetPerspectiveMatrix(okmonn::ConvertRadian(90.0f), float(window_size.x) / float(window_size.y), 0.1f, 300.0f);
    ConstantBuffer model_constant_buffer1(device, sizeof(model_matrix));
    model_constant_buffer1.UpDateConstant(&model_matrix, sizeof(model_matrix));
    std::vector<ConstantBuffer>model_constant_buffer2(pmd_material.size());
    for (std::uint32_t i = 0; i < model_constant_buffer2.size(); ++i) {
        model_constant_buffer2[i].CreateConstantBufferView(device, sizeof(pmd_material[i]));
        model_constant_buffer2[i].UpDateConstant(&pmd_material[i], sizeof(pmd_material[i]));
    }
    std::vector<ResourceBuffer>model_material_texture(pmd_material.size());
    for (std::uint32_t i = 0; i < pmd_material_texture_path.size(); ++i) {
        std::vector<std::uint8_t>material_texture;
        std::uint32_t width  = 0;
        std::uint32_t height = 0;
        if (pmd_material_texture_path[i].size() == 0) {
            width  = 4;
            height = 4;
            material_texture.resize(width * height * 4, 0xFF);
        }
        else {
            okmonn::LoadBitmapFromFile("../../rsc/モデル/" + pmd_material_texture_path[i], material_texture, &width, &height);
        }
        model_material_texture[i].CreateShaderResourceView(device, width, height);
        model_material_texture[i].UpDateResource(material_texture.data());
    }
    std::vector<okmonn::Matrix4x4>model_bone_matrix(pmd_born.size());
    ConstantBuffer model_constant_buffer3(device, std::uint32_t(sizeof(model_bone_matrix[0]) * model_bone_matrix.size()));
    model_constant_buffer3.UpDateConstant(model_bone_matrix.data(), std::uint32_t(sizeof(model_bone_matrix[0]) * model_bone_matrix.size()));
    std::unordered_map<std::string, std::vector<okmonn::MotionData>>motion;
    okmonn::LoadVmdFromFile("../../rsc/モーション/swing.vmd", motion);
    for (auto& i : motion) {
        std::sort(i.second.begin(), i.second.end(), [&](const okmonn::MotionData& a, const okmonn::MotionData& b)->bool {
            return a.frame < b.frame;
        });
    }
    
    float angle = 0.0f;
    float frame = 0;
    while (CheckMsg()) {
        std::fill(model_bone_matrix.begin(), model_bone_matrix.end(), okmonn::Matrix4x4());
        for (auto& i : motion) {
            auto bone = std::find_if(i.second.rbegin(), i.second.rend(),
                [frame](const okmonn::MotionData& m)->bool {
                    return m.frame <= frame;
                });
            if (bone == i.second.rend()) {
                continue;
            }
            auto next = bone.base();
            if (next == i.second.end()) {
                RotateBone(pmd_born[i.first], model_bone_matrix, bone->rotation);
            }
            else {
                float t = (float(frame) - bone->frame) / (next->frame - bone->frame);
                RotateBone(pmd_born[i.first], model_bone_matrix, bone->rotation, next->rotation, t);
            }
        }
        /*モーション適用の場合必要*/
        RecursionBoneMatrix(pmd_born["センター"], model_bone_matrix, okmonn::Matrix4x4());
        model_constant_buffer3.UpDateConstant(model_bone_matrix.data(), std::uint32_t(sizeof(model_bone_matrix[0]) * model_bone_matrix.size()));

        frame += 0.5f;
        if (frame >= 30.0f) {
            frame = 0.0f;
        }


        command_allocator.Reset();
        command_list.Reset(command_allocator);
        command_list.SetViewport(window_size);
        command_list.SetScissor(window_size);

        command_list.SetResourceTransitionBarrier(render_target.Get(),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);
        command_list.ClearRenderTarget(render_target, clear_color, &depth_stencil);
        command_list.ClearDepthStencil(depth_stencil);

        std::uint32_t root_param_index = 0;

        /*command_list.SetGraphicRootSignature(primitive2D_root_signature);
        command_list.SetPipeline(primitive2D_pipeline);
        command_list.SetGraphicConstantBuffer(primitive2D_constant_buffer, root_param_index++);
        command_list.DrawCall(primitive2D_vertex_buffer, primitive2D_index_buffer, sizeof(primitive2D_vertex) / sizeof(primitive2D_vertex[0]),
            sizeof(primitive_index) / sizeof(primitive_index[0]), D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

        /*command_list.SetGraphicRootSignature(texture2D_root_signature);
        command_list.SetPipeline(texture2D_pipeline);
        command_list.SetGraphicConstantBuffer(texture2D_constant_buffer, root_param_index++);
        command_list.SetGraphicResourceBuffer(texture2D_resource_buffer, root_param_index++);
        command_list.DrawCall(texture2D_vertex_buffer, texture2D_index_buffer, sizeof(texture2D_vertex) / sizeof(texture2D_vertex[0]),
            sizeof(texture_index) / sizeof(texture_index[0]), D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

        /*primitive3D_constant.world.RotationY(okmonn::ConvertRadian(angle++));
        primitive3D_constant_buffer.UpDateConstant(&primitive3D_constant, sizeof(primitive3D_constant));
        command_list.SetGraphicRootSignature(primitive3D_root_signature);
        command_list.SetPipeline(primitive3D_pipeline);
        command_list.SetGraphicConstantBuffer(primitive3D_constant_buffer, root_param_index++);
        command_list.DrawCall(primitive3D_vertex_buffer, primitive3D_index_buffer, sizeof(primitive3D_vertex) / sizeof(primitive3D_vertex[0]),
            sizeof(primitive_index) / sizeof(primitive_index[0]), D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

        /*texture3D_constant.world.RotationY(okmonn::ConvertRadian(angle++));
        texture3D_constant_buffer.UpDateConstant(&texture3D_constant, sizeof(texture3D_constant));
        command_list.SetGraphicRootSignature(texture3D_root_signature);
        command_list.SetPipeline(texture3D_pipeline);
        command_list.SetGraphicConstantBuffer(texture3D_constant_buffer, root_param_index++);
        command_list.SetGraphicResourceBuffer(texture3D_resource_buffer, root_param_index++);
        command_list.DrawCall(texture3D_vertex_buffer, texture3D_index_buffer, sizeof(texture3D_vertex) / sizeof(texture3D_vertex[0]),
            sizeof(texture_index) / sizeof(texture_index[0]), D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

        model_matrix.world.RotationY(okmonn::ConvertRadian(angle++));
        model_constant_buffer1.UpDateConstant(&model_matrix, sizeof(model_matrix));
        command_list.SetGraphicRootSignature(model_root_signature);
        command_list.SetPipeline(model_pipeline);
        command_list.SetGraphicConstantBuffer(model_constant_buffer1, root_param_index++);
        command_list.SetGraphicConstantBuffer(model_constant_buffer3, root_param_index++);
        std::uint32_t index_offset = 0;
        for (std::uint32_t i = 0; i < pmd_material.size(); ++i) {
            std::uint32_t root_index = root_param_index;
            command_list.SetGraphicConstantBuffer(model_constant_buffer2[i], root_index++);
            command_list.SetGraphicResourceBuffer(model_material_texture[i], root_index++);
            command_list.DrawCall(model_vertex_buffer, model_index_buffer, std::uint32_t(pmd_vertex.size()),
                pmd_material_index[i], D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, index_offset);
            index_offset += pmd_material_index[i];
        }

        command_list.SetResourceTransitionBarrier(render_target.Get(),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT);

        command_list.Close();
        command_queue.ExecutionCommand({ &command_list });
        fence.WaitForCommandExecution();
        swap_chain.Present();
    }

	return 0;
}