#include "Primitive3D.hlsli"

Output main(Input input)
{
    float4x4 vp = mul(perspective, view);
    input.pos   = float4(mul(input.pos, world), 1.0f);
    input.pos   = mul(vp, input.pos);
    
    Output output;
    output.sv_pos = input.pos;
    output.pos    = input.pos;
    
    return output;
}