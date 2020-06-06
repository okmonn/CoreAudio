#ifndef DIRECTX_COMMAND_LIST
#define DIRECTX_COMMAND_LIST

#include "BaseClass.h"
#include "Vector.h"

struct Device;
struct CommandAllocator;
struct RenderTarget;
struct DepthStencil;
struct RootSignature;
struct Pipeline;
struct VertexBuffer;
struct IndexBuffer;
struct ConstantBuffer;
struct ResourceBuffer;

struct CommandList :
	public BaseClass<ID3D12GraphicsCommandList5> {
public:
	/*コンストラクタ
	.Device
	.コマンドタイプ*/
	CommandList(const Device& device, const D3D12_COMMAND_LIST_TYPE& type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);

public:
	/*リセット
	.CommandAllocator*/
	void Reset(const CommandAllocator& command_allocator) const;
	/*クローズ*/
	void Close(void) const;
	/*ビューポートの設定
	.ウィンドウサイズ*/
	void SetViewport(const okmonn::Vec2& window_size) const;
	/*シザーの設定
	.ウィンドウサイズ*/
	void SetScissor(const okmonn::Vec2& window_size) const;
	/*リソースバリア
	.リソース
	.遷移前リソース状態
	.遷移後リソース状態*/
	void SetResourceTransitionBarrier(ID3D12Resource1* rsc, 
		const D3D12_RESOURCE_STATES& befor, const D3D12_RESOURCE_STATES& after);
	/*レンダーターゲットのクリア
	.RenderTarget
	.クリア色
	.DepthStencil*/
	void ClearRenderTarget(const RenderTarget& render_target, const float clear_color[4], const DepthStencil* depth_stencil);
	/*デプスステンシルのクリア
	.DepthStencil*/
	void ClearDepthStencil(const DepthStencil& depth_stencil);
	/*描画用ルートシグネチャのセット
	.RootSignature*/
	void SetGraphicRootSignature(const RootSignature& root_signature);
	/*パイプラインのセット
	.Pipeline*/
	void SetPipeline(const Pipeline& pipeline);
	/*描画用定数バッファのセット
	.ConstantBuffer
	.ルートパラメータ番号
	.リソース番号*/
	void SetGraphicConstantBuffer(const ConstantBuffer& constant_buffer, const std::uint32_t& root_param_index, const std::uint32_t& rsc_index = 0);
	/*描画用リソースバッファのセット
	.ResourceBuffer
	.ルートパラメータ番号*/
	void SetGraphicResourceBuffer(const ResourceBuffer& resource_buffer, const std::uint32_t& root_param_index);
	/*頂点バッファを基に描画コマンドを発行
	.VertexBuffer
	.頂点数
	.トポロジー種別
	.インスタンス数*/
	void DrawCall(const VertexBuffer& vertex_buffer, const std::uint32_t& vertex_num, 
		const D3D12_PRIMITIVE_TOPOLOGY& topology_type, const std::uint32_t& instance_num = 1);
	/*インデックスバッファを基に描画コマンドを発行
	.VertexBuffer
	.IndexBuffer
	.頂点数
	.インデックス数
	.トポロジー種別
	.インデックスバッファ開始オフセット
	.インスタンス数*/
	void DrawCall(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer, 
		const std::uint32_t& vertex_num, const std::uint32_t& index_num,
		const D3D12_PRIMITIVE_TOPOLOGY& topology_type, const std::uint32_t& index_buffer_offset = 0, const std::uint32_t& instance_num = 1);

private:
	/*コマンドリストの生成
	.Device
	.コマンドタイプ*/
	void CreateCommandList(const Device& device, const D3D12_COMMAND_LIST_TYPE& type);
};

#endif
