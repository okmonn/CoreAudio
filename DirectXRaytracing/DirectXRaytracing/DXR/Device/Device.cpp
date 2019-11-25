#include "Device.h"
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// 機能レベル一覧
const D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
};

// コンストラクタ
Device::Device() : 
	factory(nullptr), device(nullptr)
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug>debug = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();
#endif

	CreateFactory();
	CreateDevice();
}

// デストラクタ
Device::~Device()
{
	if (device != nullptr)
	{
		device->Release();
		device = nullptr;
	}
	if (factory != nullptr)
	{
		factory->Release();
		factory = nullptr;
	}
}

// ファクトリーの生成
void Device::CreateFactory(void)
{
	auto hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	_ASSERT(hr == S_OK);
}

// デバイスの生成
void Device::CreateDevice(void)
{
	Microsoft::WRL::ComPtr<IDXGIAdapter1>adap = nullptr;
	for (unsigned int i = 0; factory->EnumAdapters1(i, &adap) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 desc{};
		auto hr = adap->GetDesc1(&desc);
		_ASSERT(hr == S_OK);

		for (const D3D_FEATURE_LEVEL& level : levels)
		{
			if (SUCCEEDED(D3D12CreateDevice(adap.Get(), level, IID_PPV_ARGS(&device))))
			{
				//DXRのサポートチェック
				D3D12_FEATURE_DATA_D3D12_OPTIONS5 option{};
				hr = device->CheckFeatureSupport(D3D12_FEATURE::D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(option));
				_ASSERT(hr == S_OK);

				if (option.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
				{
					goto END;
				}
			}
		}
	}

END:
	/*何もしない*/;
}

// ファクトリーの取得
IDXGIFactory7* Device::Factory(void) const
{
	return factory;
}

// デバイスの取得
ID3D12Device5* Device::Dev(void) const
{
	return device;
}

// インスタンス変数の取得
Device& Device::Get(void)
{
	static Device instance;
	return instance;
}
