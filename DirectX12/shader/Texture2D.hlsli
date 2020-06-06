Texture2D<float4>tex : register(t0);
SamplerState smp     : register(s0);
float4x3 mtx         : register(b0);

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