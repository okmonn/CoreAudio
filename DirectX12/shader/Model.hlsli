Texture2D<float4> tex : register(t0);
SamplerState smp      : register(s0);

cbuffer Constant : register(b0)
{
    float4x3 world;
    float4x4 view;
    float4x4 perspective;
}
cbuffer Born : register(b1)
{
    float4x4 bone_mat[512];
}
cbuffer Material : register(b2)
{
    float4 diffuse;
    float4 supecular;
    float4 ambient;
}

struct Input
{
    float4 pos       : POSITION;
    float2 uv        : TEXCOORD;
    float4 normal    : NORMAL;
    min16uint2 bone  : BONENO;
    min16uint weight : WEIGHT;
};

struct Output
{
    float4 sv_pos    : SV_POSITION;
    float4 pos       : POSITION;
    float2 uv        : TEXCOORD;
    float4 normal    : NORMAL;
    min16uint2 bone  : BONENO;
    min16uint weight : WEIGHT;
};
