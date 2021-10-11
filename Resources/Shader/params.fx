#ifndef _PARAMS_FX_
#define _PARAMS_FX_

//packing 룰이 있음. 
//어떤 구조체를 만들 때, 실질적으로 메모리에 올라갈 때, 메모리 정렬 순서를 의미
//HLSL에서는 16byte를 넘어가선 안됨(보통 vs에선 4byte로 정렬)

// 예제
//  2 x 16byte 
//cbuffer IE
//{
//    float4 Val1;  <- 16byte
//    float2 Val2;  // starts a new vector <- 8byte
//    float2 Val3;  <- 8byte
//};

//  3 x 16byte elements
//cbuffer IE
//{
//    float2 Val1;  <- 8byte ( 8byte 공간이 남음)
//    float4 Val2;  // starts a new vector  크기가 16byte이기 때문에 2번째(결국 위에 8byte 빈공간이 남음)
//    float2 Val3;  // starts a new vector
//};

// 하드웨어 적 이슈로 보임.
// 결국 C++ 코드에서 데이터를 보낼 때, 16byte 단위로 만듦.
// 중간 중간에 쓰지 않는 변수를 넣어 16byte를 맞춤.

struct LightColor
{
    float4      diffuse;
    float4      ambient;
    float4      specular;
};

struct LightInfo
{
    LightColor  color;
    float4	    position;
    float4	    direction;
    int		    lightType;
    float	    range;
    float	    angle;
    int  	    padding;
};

cbuffer GLOBAL_PARAMS : register(b0)
{
    int         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];
}

//row_major
// - DirectX에서 기본 행렬과 Shader 문법에서 행렬 접근 순서가 다르기 때문에,
// - 그 순서를 DirectX 방식으로 맞춰주기 위함.(row(행)을 기준으로 읽어나감)
cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
    row_major matrix g_matViewInv;
};

cbuffer MATERIAL_PARAMS : register(b2)
{
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    int     g_tex_on_0;
    int     g_tex_on_1;
    int     g_tex_on_2;
    int     g_tex_on_3;
    float2  g_vec2_0;
    float2  g_vec2_1;
    float2  g_vec2_2;
    float2  g_vec2_3;
    float4  g_vec4_0;
    float4  g_vec4_1;
    float4  g_vec4_2;
    float4  g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
};

// Texture를 받음.
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);

StructuredBuffer<Matrix> g_mat_bone : register(t7);

SamplerState g_sam_0 : register(s0);

#endif