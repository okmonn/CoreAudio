#include "Model.hlsli"

/*内積
.ベクトル1
.ベクトル2
return 内積*/
float Dot(float3 vec1, float3 vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

/*反射ベクトル
.進行ベクトル
.衝突物の法線ベクトル
return 反射ベクトル*/
float3 Reflect(float3 vec, float3 normal)
{
    return vec + (2.0f * dot(-vec, normal) * normal);
}

float4 main(Output output) : SV_TARGET
{
    /*光源色*/
    float3 light_color = float3(1.0f, 1.0f, 1.0f);
    /*視線ベクトル*/
    float3 eye_vec = normalize(output.pos.xyz - float3(0.0f, 10.0f, -15.0f));
    /*光源ベクトル*/
    float3 light_vec = normalize(float3(-1.0f, 1.0f, -1.0f));
    
    /*反射ベクトル*/
    float3 ref_vec = Reflect(light_vec, output.normal.xyz);
    /*スペキュラの度合い*/
    float3 spec_level = pow(saturate(Dot(ref_vec, eye_vec)), supecular.w);
    /*光源ベクトルと法線ベクトルとの内積*/
    float bright = saturate(Dot(light_vec, output.normal.xyz));
    /*ランバート*/
    float lambert = saturate(acos(bright) / acos(-1.0f));

    /*色*/
    float3 color = saturate((diffuse.xyz * lambert) + (supecular.xyz * spec_level) + (ambient.xyz * light_color));

    return tex.Sample(smp, output.uv) * float4(color, diffuse.w);
}