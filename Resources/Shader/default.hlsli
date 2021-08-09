#ifndef _DEFAULT_HLSLI_
#define _DEFAULT_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;   //(SV -> System Value)가 붙으면 예약된 것임.
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
};

// vertex 계산
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);    // 투영 좌표계까지
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.0f), g_matWV).xyz; //(위치라서 w값을 1로)
    output.viewNormal = normalize(mul(float4(input.normal, 0.0f), g_matWV).xyz);    //(방향을 위해서 0으로)

    return output;
}


// pixel 단위의 색 연산
float4 PS_Main(VS_OUT input) : SV_Target
{
    //float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);
   
    LightColor totalColor = (LightColor)0.0f;

    // 광원의 개수만큼 계산    
    for (int i = 0; i < g_lightCount; ++i)
    {
        LightColor lc = CalculateLightColor(i, input.viewNormal, input.viewPos);
        totalColor.diffuse += lc.diffuse;
        totalColor.ambient += lc.ambient;
        totalColor.specular += lc.specular;
    }

    color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;

    return color;
}

#endif