#ifndef _DEFAULT_HLSLI_
#define _DEFAULT_HLSLI_

#include "params.hlsli"

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

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);    // ���� ��ǥ�����
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.0f), g_matVW).xyz; //(��ġ�� 1��)
    output.viewNormal = normalize(mul(float4(input.normal, 0.0f), g_matWV).xyz);    //(������ ���ؼ� 0����)

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);

    return color;
}

#endif