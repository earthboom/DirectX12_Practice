#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"

struct VS_IN
{
	float3 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_Position;	// System Value�� Pixel Shader�� �Ѿ��, clip position�� �ƴ� ���� Pixel�� ��ǥ�� ��.
	float4 clipPos : POSITION;	// clip position�� ���� ������
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0.0f;

	output.pos = mul(float4(input.pos, 1.0f), g_matWVP);	// clip space ��ȯ
	output.clipPos = output.pos;

	return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
	// z ���� w�� ������ Projection positoin���� ��ȯ�Ͽ� ��ȯ
	return float4(input.clipPos.z / input.clipPos.w, 0.0f, 0.0f, 0.0f);
}

#endif