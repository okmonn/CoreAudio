cbuffer Constant : register(b0)
{
    float4 color;
    float4x3 world;
    float4x4 view;
    float4x4 perspective;
}

struct Input
{
    float4 pos : POSITION;
};

struct Output
{
    float4 sv_pos : SV_POSITION;
    float4 pos    : POSITION;
};