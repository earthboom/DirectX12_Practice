#ifndef _PARAMS_FX_
#define _PARAMS_FX_

//packing ���� ����. 
//� ����ü�� ���� ��, ���������� �޸𸮿� �ö� ��, �޸� ���� ������ �ǹ�
//HLSL������ 16byte�� �Ѿ�� �ȵ�(���� vs���� 4byte�� ����)

// ����
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
//    float2 Val1;  <- 8byte ( 8byte ������ ����)
//    float4 Val2;  // starts a new vector  ũ�Ⱑ 16byte�̱� ������ 2��°(�ᱹ ���� 8byte ������� ����)
//    float2 Val3;  // starts a new vector
//};

// �ϵ���� �� �̽��� ����.
// �ᱹ C++ �ڵ忡�� �����͸� ���� ��, 16byte ������ ����.
// �߰� �߰��� ���� �ʴ� ������ �־� 16byte�� ����.

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
// - DirectX���� �⺻ ��İ� Shader �������� ��� ���� ������ �ٸ��� ������,
// - �� ������ DirectX ������� �����ֱ� ����.(row(��)�� �������� �о��)
cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer MATERIAL_PARAMS : register(b2)
{
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    int     g_int_4;
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    float   g_float_4;
    int     g_tex_on_0;
    int     g_tex_on_1;
    int     g_tex_on_2;
    int     g_tex_on_3;
    int     g_tex_on_4;
};

// Texture�� ����.
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);

SamplerState g_sam_0 : register(s0);

#endif