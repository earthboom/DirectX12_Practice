#ifndef _FORWARD_FX_
#define _FORWARD_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
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

    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);    // 투영 좌표계까지
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.0f), g_matWV).xyz; //(위치라서 w값을 1로)
    output.viewNormal = normalize(mul(float4(input.normal, 0.0f), g_matWV).xyz);    //(방향을 위해서 0으로)
    
    output.viewTangent = normalize(mul(float4(input.tangent, 0.0f), g_matWV).xyz);  // tangent 계산(Normal mapping을 위함)
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}


// pixel 단위의 색 연산
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if(g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1) // normal map 이미지를 샘플링 했다면
    {
        // [0, 255] 범위에서 [0, 1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;

        // [0, 1] 범위에서 [-1, 1]로 변환 (방향 벡터로 변환할 필요가 있음)
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.0f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };  //tangent공간 행렬 생성
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN)); //view 공간 normal 생성!
    }
   
    LightColor totalColor = (LightColor)0.0f;

    // 광원의 개수만큼 계산    
    for (int i = 0; i < g_lightCount; ++i)
    {
        LightColor lc = CalculateLightColor(i, viewNormal, input.viewPos);
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