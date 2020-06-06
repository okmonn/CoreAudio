#ifndef DIRECTX_ROOT_SIGNATURE
#define DIRECTX_ROOT_SIGNATURE

#include "BaseClass.h"
#include <vector>

struct Device;

struct RootSignature :
	public BaseClass<ID3D12RootSignature> {
public:
	/*コンストラクタ
	.Device
	.ルートパラメータリスト
	.サンプラーリスト*/
	RootSignature(const Device& device,
		const std::vector<D3D12_ROOT_PARAMETER1>& param, const std::vector<D3D12_STATIC_SAMPLER_DESC>& sampler);

private:
	/*ルートシグネチャの生成
	.Device
	.ルートパラメータリスト
	.サンプラーリスト*/
	void CreateRootSignature(const Device& device,
		const std::vector<D3D12_ROOT_PARAMETER1>& param, const std::vector<D3D12_STATIC_SAMPLER_DESC>& sampler);
};

#endif
