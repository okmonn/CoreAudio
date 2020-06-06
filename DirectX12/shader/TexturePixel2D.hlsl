#include "Texture2D.hlsli"

float4 main(Output output) : SV_TARGET
{
    return tex.Sample(smp, output.uv);
}