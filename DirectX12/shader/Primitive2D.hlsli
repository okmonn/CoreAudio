cbuffer Constant : register(b0)
{
    float4 color;
    float4x3 mtx;
}

struct Input
{
    float4 pos    : POSITION;
};

struct Output
{
    float4 sv_pos : SV_POSITION;
    float4 pos    : POSITION;
};