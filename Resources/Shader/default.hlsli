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
    float4 pos : SV_Position;   //(SV -> System Value)�� ������ ����� ����.
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
};

// vertex ���
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);    // ���� ��ǥ�����
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.0f), g_matWV).xyz; //(��ġ�� w���� 1��)
    output.viewNormal = normalize(mul(float4(input.normal, 0.0f), g_matWV).xyz);    //(������ ���ؼ� 0����)

    return output;
}


// pixel ������ �� ����
float4 PS_Main(VS_OUT input) : SV_Target
{
    //float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);
   
    LightColor totalColor = (LightColor)0.0f;

    // ������ ������ŭ ���    
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