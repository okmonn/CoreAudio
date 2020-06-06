#include "../include/Pipeline.h"
#include "../include/Device.h"
#include "../include/SwapChain.h"
#include "../include/RootSignature.h"
#include <cassert>

Pipeline::Pipeline(const Device& device, const SwapChain& swap_chain, const RootSignature& root_signature, const std::vector<ID3DBlob*>& shader,
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& input, const D3D12_PRIMITIVE_TOPOLOGY_TYPE& topology_type, const bool& depth_flag)
{
	CreatePipeline(device, swap_chain, root_signature,
		shader, input, topology_type, depth_flag);
}

void Pipeline::CreatePipeline(const Device& device, const SwapChain& swap_chain, const RootSignature& root_signature, const std::vector<ID3DBlob*>& shader, 
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& input, const D3D12_PRIMITIVE_TOPOLOGY_TYPE& topology_type, const bool& depth_flag)
{
	D3D12_RASTERIZER_DESC raster{};
	raster.AntialiasedLineEnable = false;
	raster.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	raster.CullMode              = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	raster.DepthBiasClamp        = 0.0f;
	raster.DepthBiasClamp        = 0.0f;
	raster.DepthClipEnable       = false;
	raster.FillMode              = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
	raster.ForcedSampleCount     = 0;
	raster.FrontCounterClockwise = false;
	raster.MultisampleEnable     = false;
	raster.SlopeScaledDepthBias  = 0.0f;

	D3D12_RENDER_TARGET_BLEND_DESC blend{};
	blend.BlendEnable           = true;
	blend.BlendOp               = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	blend.BlendOpAlpha          = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	blend.DestBlend             = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
	blend.DestBlendAlpha        = D3D12_BLEND::D3D12_BLEND_ZERO;
	blend.LogicOp               = D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
	blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
	blend.SrcBlend              = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
	blend.SrcBlendAlpha         = D3D12_BLEND::D3D12_BLEND_ONE;

	D3D12_BLEND_DESC render;
	render.AlphaToCoverageEnable  = false;
	render.IndependentBlendEnable = false;
	for (uint32_t i = 0U; i < swap_chain.GetBackBufferNum(); ++i) {
		render.RenderTarget[i] = blend;
	}

	auto itr = shader.rbegin();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	desc.BlendState                                    = render;
	desc.CachedPSO                                     = {};
	desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.BackFace.StencilFailOp      = D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.BackFace.StencilFunc        = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
	desc.DepthStencilState.BackFace.StencilPassOp      = D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR;
	if (depth_flag == true) {
		desc.DepthStencilState.DepthEnable    = true;
		desc.DepthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
	}
	desc.DS.BytecodeLength                             = 0;
	desc.DS.pShaderBytecode                            = nullptr;
	desc.DSVFormat                                     = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	desc.Flags                                         = D3D12_PIPELINE_STATE_FLAGS::D3D12_PIPELINE_STATE_FLAG_NONE;
	if (shader.size() > 2) {
		desc.GS.BytecodeLength = (*itr)->GetBufferSize();
		desc.GS.pShaderBytecode = (*itr--)->GetBufferPointer();
	}
	else {
		desc.GS.BytecodeLength  = 0;
		desc.GS.pShaderBytecode = nullptr;
	}
	desc.HS.BytecodeLength              = 0;
	desc.HS.pShaderBytecode             = nullptr;
	desc.IBStripCutValue                = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE::D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	desc.InputLayout.NumElements        = uint32_t(input.size());
	desc.InputLayout.pInputElementDescs = input.data();
	desc.NodeMask                       = 0;
	desc.NumRenderTargets               = swap_chain.GetBackBufferNum();
	desc.PrimitiveTopologyType          = topology_type;
	desc.pRootSignature                 = root_signature.Get();
	desc.PS.BytecodeLength              = (*itr)->GetBufferSize();
	desc.PS.pShaderBytecode             = (*itr++)->GetBufferPointer();
	desc.RasterizerState                = raster;

	DXGI_SWAP_CHAIN_DESC1 swap_desc{};
	auto hr = swap_chain.Get()->GetDesc1(&swap_desc);
	assert(hr == S_OK);
	for (uint32_t i = 0U; i < swap_chain.GetBackBufferNum() - 1; ++i) {
		desc.RTVFormats[i] = swap_desc.Format;
	}
	desc.SampleDesc         = { 1, 0 };
	desc.SampleMask         = 0xFFFFFFFF;
	desc.StreamOutput       = {};
	desc.VS.BytecodeLength  = (*itr)->GetBufferSize();
	desc.VS.pShaderBytecode = (*itr)->GetBufferPointer();

	hr = device.Get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}
