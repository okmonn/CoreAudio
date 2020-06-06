Texture2D<float4>tex : register(t0);
SamplerState smp     : register(s0);

cbuffer Constant : register(b0)
{
    float4x3 world;
    float4x4 view;
    float4x4 perspective;
}

struct Input
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD;
};

struct Output
{
    float4 sv_pos : SV_POSITION;
    float4 pos    : POSITION;
    float2 uv     : TEXCOORD;
};