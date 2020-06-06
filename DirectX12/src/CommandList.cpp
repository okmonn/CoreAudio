#include "../include/CommandList.h"
#include "../include/Device.h"
#include "../include/CommandAllocator.h"
#include "../include/RenderTarget.h"
#include "../include/DepthStencil.h"
#include "../include/RootSignature.h"
#include "../include/Pipeline.h"
#include "../include/VertexBuffer.h"
#include "../include/IndexBuffer.h"
#include "../include/ConstantBuffer.h"
#include "../include/ResourceBuffer.h"
#include <cassert>

CommandList::CommandList(const Device& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	CreateCommandList(device, type);
}

void CommandList::CreateCommandList(const Device& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	auto hr = device.Get()->CreateCommandList1(0U, type,
		D3D12_COMMAND_LIST_FLAGS::D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(GetAddress()));
	assert(hr == S_OK);
}

void CommandList::Reset(const CommandAllocator& command_allocator) const
{
	auto hr = Get()->Reset(command_allocator.Get(), nullptr);
	assert(hr == S_OK);
}

void CommandList::Close(void) const
{
	auto hr = Get()->Close();
	assert(hr == S_OK);
}

void CommandList::SetViewport(const okmonn::Vec2& window_size) const
{
	D3D12_VIEWPORT desc{};
	desc.Height   = float(window_size.y);
	desc.MaxDepth = 1.0f;
	desc.MinDepth = 0.0f;
	desc.TopLeftX = 0.0f;
	desc.TopLeftY = 0.0f;
	desc.Width    = float(window_size.x);

	Get()->RSSetViewports(1, &desc);
}

void CommandList::SetScissor(const okmonn::Vec2& window_size) const
{
	RECT desc{};
	desc.bottom = window_size.y;
	desc.left   = 0;
	desc.right  = window_size.x;
	desc.top    = 0;

	Get()->RSSetScissorRects(1, &desc);
}

void CommandList::SetResourceTransitionBarrier(ID3D12Resource1* rsc, const D3D12_RESOURCE_STATES& befor, const D3D12_RESOURCE_STATES& after)
{
	D3D12_RESOURCE_BARRIER desc{};
	desc.Flags                  = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Transition.pResource   = rsc;
	desc.Transition.StateAfter  = after;
	desc.Transition.StateBefore = befor;
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	desc.Type                   = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	Get()->ResourceBarrier(1, &desc);
}

void CommandList::ClearRenderTarget(const RenderTarget& render_target, const float clear_color[4], const DepthStencil* depth_stencil)
{
	ComPtr<ID3D12Device6>device;
	auto hr = Get()->GetDevice(IID_PPV_ARGS(&device));
	assert(hr == S_OK);

	auto handle = render_target.heap.Get()->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += device->GetDescriptorHandleIncrementSize(render_target.heap.Get()->GetDesc().Type)
		* render_target.GetBackBufferIndex();

	if (depth_stencil == nullptr) {
		Get()->OMSetRenderTargets(1, &handle, false, nullptr);
	}
	else {
		Get()->OMSetRenderTargets(1, &handle, false, &depth_stencil->heap.Get()->GetCPUDescriptorHandleForHeapStart());
	}
	Get()->ClearRenderTargetView(handle, clear_color, 0, nullptr);
}

void CommandList::ClearDepthStencil(const DepthStencil& depth_stencil)
{
	Get()->ClearDepthStencilView(depth_stencil.heap.Get()->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	Get()->ClearDepthStencilView(depth_stencil.heap.Get()->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void CommandList::SetGraphicRootSignature(const RootSignature& root_signature)
{
	Get()->SetGraphicsRootSignature(root_signature.Get());
}

void CommandList::SetPipeline(const Pipeline& pipeline)
{
	Get()->SetPipelineState(pipeline.Get());
}

void CommandList::SetGraphicConstantBuffer(const ConstantBuffer& constant_buffer, const std::uint32_t& root_param_index, const std::uint32_t& rsc_index)
{
	ComPtr<ID3D12Device6>device;
	auto hr = Get()->GetDevice(IID_PPV_ARGS(&device));
	assert(hr == S_OK);

	auto heap = constant_buffer.heap.Get();
	Get()->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)&heap);
	auto handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += device->GetDescriptorHandleIncrementSize(heap->GetDesc().Type) * rsc_index;
	Get()->SetGraphicsRootDescriptorTable(root_param_index, handle);
}

void CommandList::SetGraphicResourceBuffer(const ResourceBuffer& resource_buffer, const std::uint32_t& root_param_index)
{
	auto heap = resource_buffer.heap.Get();
	Get()->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)&heap);
	Get()->SetGraphicsRootDescriptorTable(root_param_index, resource_buffer.heap.Get()->GetGPUDescriptorHandleForHeapStart());
}

void CommandList::DrawCall(const VertexBuffer& vertex_buffer, const std::uint32_t& vertex_num, const D3D12_PRIMITIVE_TOPOLOGY& topology_type, const std::uint32_t& instance_num)
{
	D3D12_VERTEX_BUFFER_VIEW desc{};
	desc.BufferLocation = vertex_buffer.Get()->GetGPUVirtualAddress();
	desc.SizeInBytes    = uint32_t(vertex_buffer.Get()->GetDesc().Width);
	desc.StrideInBytes  = desc.SizeInBytes / vertex_num;

	Get()->IASetVertexBuffers(0, 1, &desc);
	Get()->IASetPrimitiveTopology(topology_type);
	Get()->DrawInstanced(vertex_num, instance_num, 0, 0);
}

void CommandList::DrawCall(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer, 
	const std::uint32_t& vertex_num, const std::uint32_t& index_num, const D3D12_PRIMITIVE_TOPOLOGY& topology_type, 
	const std::uint32_t& index_buffer_offset, const std::uint32_t& instance_num)
{
	{
		D3D12_VERTEX_BUFFER_VIEW desc{};
		desc.BufferLocation = vertex_buffer.Get()->GetGPUVirtualAddress();
		desc.SizeInBytes    = uint32_t(vertex_buffer.Get()->GetDesc().Width);
		desc.StrideInBytes  = desc.SizeInBytes / vertex_num;
		Get()->IASetVertexBuffers(0, 1, &desc);
	}
	{
		D3D12_INDEX_BUFFER_VIEW desc{};
		desc.BufferLocation = index_buffer.Get()->GetGPUVirtualAddress();
		desc.Format         = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
		desc.SizeInBytes    = std::uint32_t(index_buffer.Get()->GetDesc().Width);
		Get()->IASetIndexBuffer(&desc);
	}

	Get()->IASetPrimitiveTopology(topology_type);
	Get()->DrawIndexedInstanced(index_num, instance_num, index_buffer_offset, 0, 0);
}
