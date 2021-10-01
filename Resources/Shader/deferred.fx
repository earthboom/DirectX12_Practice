#ifndef _DEFERRED_FX_
#define _DEFERRED_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;

    row_major matrix matWorld : W;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint instancID : SV_InstanceID; // Instance ID를 지정
};

struct VS_OUT
{
    float4 pos : SV_Position;   //(SV -> System Value)가 붙으면 예약된 것임.
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

// vertex 계산
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    if (g_int_0 == 1)    // Instance를 의미
    {
        output.pos = mul(float4(input.pos, 1.0f), input.matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.0f), input.matWV).xyz; //(위치라서 w값을 1로)
        output.viewNormal = normalize(mul(float4(input.normal, 0.0f), input.matWV).xyz);    //(방향을 위해서 0으로)

        output.viewTangent = normalize(mul(float4(input.tangent, 0.0f), input.matWV).xyz);  // tangent 계산(Normal mapping을 위함)
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }
    else
    {
        output.pos = mul(float4(input.pos, 1.0f), g_matWVP);    // 투영 좌표계까지
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.0f), g_matWV).xyz; //(위치라서 w값을 1로)
        output.viewNormal = normalize(mul(float4(input.normal, 0.0f), g_matWV).xyz);    //(방향을 위해서 0으로)

        output.viewTangent = normalize(mul(float4(input.tangent, 0.0f), g_matWV).xyz);  // tangent 계산(Normal mapping을 위함)
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }

    return output;
}


//forward rendering과는 다르게 여러 타겟을 대상으로 함.
struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};


// pixel 단위의 색 연산
PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1)
    {
        // (0, 255) 범위에서 (0, 1)로 변환 작업
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        //(0, 1) 범위에서 (-1, 1)로 변환 작업
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.0f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    output.position = float4(input.viewPos.xyz, 0.0f);
    output.normal = float4(viewNormal.xyz, 0.0f);
    output.color = color;

    return output;
}

#endif