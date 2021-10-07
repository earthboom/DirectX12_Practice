#ifndef _TERRAIN_FX_
#define _TERRAIN_FX_

#include "params.fx"
#include "utils.fx"

//=============
// Vertex Shader
//=============

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = input;

    return output;
}

//=============
// Hull Shader
//=============

// SV_TessFactor : 삼각형의 각변을 나눌 개수
// SV_InsideTessFactor : 내부에 삼각형 생성 개수
struct PatchTess
{
    float edgeTess[3] : SV_TessFactor;
    float insideTess : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

// Constant HS
PatchTess ConstantHS(InputPatch<VS_OUT, 3> input, int patchID : SV_PrimitiveID)
{
    PatchTess output = (PatchTess)0.f;

    // 테셀레이션이 적용되는 최소, 최대 카메라 거리
    float minDistance = g_vec2_1.x;     // 테셀레이션 최고 레벨이 될 최소 거리
    float maxDistance = g_vec2_1.y;     // 테셀레이션 최하 레벨이 될 최대 거리

    // edge : 해당 정점과 마주보는 변을 의미. (정점 0의 edge는 정점 2, 1의 변)
    // 해당 edge의 중앙 위치를 구함.
    float3 edge0Pos = (input[1].pos + input[2].pos) / 2.f;
    float3 edge1Pos = (input[2].pos + input[0].pos) / 2.f;
    float3 edge2Pos = (input[0].pos + input[1].pos) / 2.f;

    // world 좌표계로 전환.
    edge0Pos = mul(float4(edge0Pos, 1.f), g_matWorld).xyz;
    edge1Pos = mul(float4(edge1Pos, 1.f), g_matWorld).xyz;
    edge2Pos = mul(float4(edge2Pos, 1.f), g_matWorld).xyz;

    // edge와 카메라의 거리를 비교하여 테셀레이션 레벨을 정함.
    float edge0TessLevel = CalculateTessLevel(g_vec4_0.xyz, edge0Pos, minDistance, maxDistance, 4.f);
    float edge1TessLevel = CalculateTessLevel(g_vec4_0.xyz, edge1Pos, minDistance, maxDistance, 4.f);
    float edge2TessLevel = CalculateTessLevel(g_vec4_0.xyz, edge2Pos, minDistance, maxDistance, 4.f);

    output.edgeTess[0] = edge0TessLevel;
    output.edgeTess[1] = edge1TessLevel;
    output.edgeTess[2] = edge2TessLevel;
    output.insideTess = edge2TessLevel;

    return output;
}

// Control Point HS
// SV_OutputControlPointID : 현재 호출된 Control Point(vertex)를 의미
// SV_PrimitiveID : 현재의 Control Point들을 포함하는 Patch를 의미
[domain("tri")]                     // 패치의 종류 (tri, quad, isoline)
[partitioning("fractional_odd")]    // subdivision mode (integer 소수점 무시, fractional_even, fractional_odd)
[outputtopology("triangle_cw")]     // (triangle_cw, triangle_ccw, line)
[outputcontrolpoints(3)]            // 하나의 입력 패치에 대해, HS가 출력할 제어점 개수
[patchconstantfunc("ConstantHS")]   // ConstantHS 함수 이름
HS_OUT HS_Main(InputPatch<VS_OUT, 3> input, int vertexIdx : SV_OutputControlPointID, int patchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT)0.0f;

    output.pos = input[vertexIdx].pos;
    output.uv = input[vertexIdx].uv;

    return output;
}

// [Terrain Shader]
// g_int_1      : TileX - X축 타일 사이즈
// g_int_2      : TileZ - Z축 타일 사이즈
// g_float_0    : Max Tessellation Level - 최대 테셀레이션 레벨
// g_vec2_0     : HeightMap Resolution - 높이맵
// g_vec2_1     : Min/Max Tessellation Distance
// g_vec4_0     : Camera Position
// g_tex_0      : Diffuse Texture
// g_tex_1      : Normal Texture
// g_tex_2      : HeightMap Texture

//=============
// Domain Shader
//=============

// 좌표 계산을 여기서 함.
struct DS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

// SV_DomainLocation : 해당 정점에 대해, 각 Control Point들의 비율을 의미함.
[domain("tri")]
DS_OUT DS_Main(const OutputPatch<HS_OUT, 3> input, float3 location : SV_DomainLocation, PatchTess patch)
{
    DS_OUT output = (DS_OUT)0.f;

    float3 localPos = input[0].pos * location[0] + input[1].pos * location[1] + input[2].pos * location[2];
    float2 uv = input[0].uv * location[0] + input[1].uv * location[1] + input[2].uv * location[2];
    
    int tileCountX = g_int_1;
    int tileCountZ = g_int_2;
    int mapWidth = g_vec2_0.x;
    int mapHeight = g_vec2_0.y;

    float2 fullUV = float2(uv.x / (float)tileCountX, uv.y / (float)tileCountZ); // terrain greed의 전체 uv를 계산(0~1)
    // Pixel Shader를 제외하고 Texture에서 원하는 값을 가져올 때는 SampleLevel을 사용
    float height = g_tex_2.SampleLevel(g_sam_0, fullUV, 0).x;   // height map에서 값을 추출함.

    // 높이맵 높이 적용
    localPos.y = height;

    float2 deltaUV = float2(1.f / mapWidth, 1.f / mapHeight);
    float2 deltaPos = float2(tileCountX * deltaUV.x, tileCountZ * deltaUV.y);

    float upHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x, fullUV.y - deltaUV.y), 0).x;
    float downHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x, fullUV.y + deltaUV.y), 0).x;
    float rightHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x + deltaUV.x, fullUV.y), 0).x;
    float leftHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x - deltaUV.x, fullUV.y), 0).x;

    float3 localTangent = float3(localPos.x + deltaPos.x, rightHeight, localPos.z) - float3(localPos.x - deltaPos.x, leftHeight, localPos.z);
    float3 localBinormal = float3(localPos.x, upHeight, localPos.z + deltaPos.y) - float3(localPos.x, downHeight, localPos.z - deltaPos.y);

    output.pos = mul(float4(localPos, 1.f), g_matWVP);
    output.viewPos = mul(float4(localPos, 1.f), g_matWV).xyz;

    output.viewTangent = normalize(mul(float4(localTangent, 0.f), g_matWV)).xyz;
    output.viewBinormal = normalize(mul(float4(localBinormal, 0.f), g_matWV)).xyz;
    output.viewNormal = normalize(cross(output.viewBinormal, output.viewTangent));

    output.uv = uv;

    return output;
}

//=============
// Pixel Shader
//=============

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(DS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (g_tex_on_0 == 1)
    color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1 == 1)
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