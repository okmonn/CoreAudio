#include "../include/RootSignature.h"
#include "../include/Device.h"
#include <cassert>

RootSignature::RootSignature(const Device& device, const std::vector<D3D12_ROOT_PARAMETER1>& param, const std::vector<D3D12_STATIC_SAMPLER_DESC>& sampler)
{
	CreateRootSignature(device, param, sampler);
}

void RootSignature::CreateRootSignature(const Device& device, const std::vector<D3D12_ROOT_PARAMETER1>& param, const std::vector<D3D12_STATIC_SAMPLER_DESC>& sampler)
{
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc{};
    desc.Desc_1_1.Flags             = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.Desc_1_1.NumParameters     = std::uint32_t(param.size());
    desc.Desc_1_1.NumStaticSamplers = std::uint32_t(sampler.size());
    if (param.size() != 0U) {
        desc.Desc_1_1.pParameters = param.data();
    }
    else {
        desc.Desc_1_1.pParameters = nullptr;
    }
    if (sampler.size() != 0U) {
        desc.Desc_1_1.pStaticSamplers = sampler.data();
    }
    else {
        desc.Desc_1_1.pStaticSamplers = nullptr;
    }
    desc.Version = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1;

    ComPtr<ID3DBlob>blob;
    auto hr = D3D12SerializeVersionedRootSignature(&desc, &blob, nullptr);
    assert(hr == S_OK);

    hr = device.Get()->CreateRootSignature(0U, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(GetAddress()));
    assert(hr == S_OK);
}
