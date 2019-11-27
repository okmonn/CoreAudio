#include "Header.hlsli"

// ルートシグネチャの情報
#define RS "RootFlags(LOCAL_ROOT_SIGNATURE),"\
		   "DescriptorTable(UAV(u0, numDescriptors = 1, space = 0, offset = DESCRIPTOR_RANGE_OFFSET_APPEND), visibility = SHADER_VISIBILITY_ALL),"\
		   "DescriptorTable(SRV(t0, numDescriptors = 1, space = 0, offset = DESCRIPTOR_RANGE_OFFSET_APPEND), visibility = SHADER_VISIBILITY_ALL)"

// レイジェネレーション
[RootSignature(RS)]
[shader("raygeneration")]
void Ray()
{
	RayPayload payload;

}
