#include "Texture2D.hlsli"

Output main(Input input)
{
    input.pos = float4(mul(input.pos, mtx), 1.0f);
    
    Output output;
    output.sv_pos = input.pos;
    output.pos    = input.pos;
    output.uv     = input.uv;
    
    return output;
}