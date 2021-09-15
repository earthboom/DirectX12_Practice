#ifndef _SKYBOX_HLSLI_
#define _SKYBOX_HLSLI_

#include "params.hlsli"

struct VS_IN
{
    float3 localPos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;   //(SV -> System Value)�� ������ ����� ����.
    float2 uv : TEXCOORD;
};

// vertex ���
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    //Translation�� ���� �ʰ� Rotation�� ����
    float4 viewPos = mul(float4(input.localPos, 0.0f), g_matView);
    float4 clipSpacePos = mul(viewPos, g_matProjection);

    // w / w = 1 �̱� ������ �׻� ���̰� 1�� ����
    output.pos = clipSpacePos.xyww;
    output.uv = input.uv;

    return output;
}


// pixel ������ �� ����
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    return color;
}

#endif