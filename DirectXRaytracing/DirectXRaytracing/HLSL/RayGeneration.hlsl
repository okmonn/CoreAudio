#include "Header.hlsli"

// ルートシグネチャの情報
#define RS "RootFlags(LOCAL_ROOT_SIGNATURE),"\n
			 "DescriptorTable(UAV(u0, numDescriptors = 1, space = 0, offset = DESCRIPTOR_RANGE_OFFSET_APPEND), visibility = SHADER_VISIBILITY_ALL),"\n
			 "DescriptorTable(SRV(t0, numDescriptors = 1, space = 0, offset = DESCRIPTOR_RANGE_OFFSET_APPEND), visibility = SHADER_VISIBILITY_ALL),"\n

// レイジェネレーション
[RootSignature(RS)]
[shader("raygeneration")]
void Ray()
{
	RayPayload payload;

}
