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
    float4 pos : SV_Position;   //(SV -> System Value)가 붙으면 예약된 것임.
    float2 uv : TEXCOORD;
};

// vertex 계산
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    //Translation은 하지 않고 Rotation만 적용
    float4 viewPos = mul(float4(input.localPos, 0.0f), g_matView);
    float4 clipSpacePos = mul(viewPos, g_matProjection);

    // w / w = 1 이기 때문에 항상 깊이가 1로 유지
    output.pos = clipSpacePos.xyww;
    output.uv = input.uv;

    return output;
}


// pixel 단위의 색 연산
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    return color;
}

#endif