#include "../include/Device.h"
#include <cstdint>
#include <cassert>

namespace {
	/*‹@”\ƒŒƒxƒ‹*/
	const D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
	};
}

Device::Device()
{
#ifdef _DEBUG
	ComPtr<ID3D12Debug3>debug;
	auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	assert(hr == S_OK);
	debug->EnableDebugLayer();
#endif

	CreateDevice();
}

void Device::CreateDevice(void)
{
	ComPtr<IDXGIFactory7>factory;
	std::uint32_t debug_flag = 0U;
#ifdef _DEBUG
	debug_flag = DXGI_CREATE_FACTORY_DEBUG;
#endif
	auto hr = CreateDXGIFactory2(debug_flag, IID_PPV_ARGS(&factory));
	assert(hr == S_OK);

	ComPtr<IDXGIAdapter4>adaptor;
	DXGI_ADAPTER_DESC3 desc{};
	for (std::uint32_t i = 0U; factory->EnumAdapters1(i, (IDXGIAdapter1**)&adaptor) != DXGI_ERROR_NOT_FOUND; ++i) {
		hr = adaptor->GetDesc3(&desc);
		assert(hr == S_OK);

		for (const D3D_FEATURE_LEVEL& level : levels) {
			hr = D3D12CreateDevice(adaptor.Get(), level, IID_PPV_ARGS(GetAddress()));
			if (SUCCEEDED(hr)) {
				return;
			}
		}
	}

	hr = factory->EnumWarpAdapter(IID_PPV_ARGS(&adaptor));
	assert(hr == S_OK);

	for (const D3D_FEATURE_LEVEL& level : levels) {
		hr = D3D12CreateDevice(adaptor.Get(), level, IID_PPV_ARGS(GetAddress()));
		if (SUCCEEDED(hr)) {
			return;
		}
	}
}
