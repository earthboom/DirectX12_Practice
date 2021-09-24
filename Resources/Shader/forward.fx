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
    float4 pos : SV_Position;   //(SV -> System Value)�� ������ ����� ����.
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

// vertex ���
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.0f), g_matWVP);    // ���� ��ǥ�����
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.0f), g_matWV).xyz; //(��ġ�� w���� 1��)
    output.viewNormal = normalize(mul(float4(input.normal, 0.0f), g_matWV).xyz);    //(������ ���ؼ� 0����)
    
    output.viewTangent = normalize(mul(float4(input.tangent, 0.0f), g_matWV).xyz);  // tangent ���(Normal mapping�� ����)
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}


// pixel ������ �� ����
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if(g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1) // normal map �̹����� ���ø� �ߴٸ�
    {
        // [0, 255] �������� [0, 1]�� ��ȯ
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;

        // [0, 1] �������� [-1, 1]�� ��ȯ (���� ���ͷ� ��ȯ�� �ʿ䰡 ����)
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.0f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };  //tangent���� ��� ����
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN)); //view ���� normal ����!
    }
   
    LightColor totalColor = (LightColor)0.0f;

    // ������ ������ŭ ���    
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