#include "Model.hlsli"

Output main(Input input)
{
    float w = input.weight / 100.0f;
    float4x4 mat = bone_mat[input.bone.x] * w + bone_mat[input.bone.y] * (1.0f - w);
    input.pos = mul(mat, input.pos);
    
    float4x4 vp  = mul(perspective, view);
    input.pos    = float4(mul(input.pos, world), 1.0f);
    input.pos    = mul(vp, input.pos);
    input.normal = float4(mul(input.normal, world), 1.0f);
    
    Output output;
    output.sv_pos = input.pos;
    output.pos    = input.pos;
    output.uv     = input.uv;
    output.normal = input.normal;
    output.bone   = input.bone;
    output.weight = input.weight;
    
    return output;
}