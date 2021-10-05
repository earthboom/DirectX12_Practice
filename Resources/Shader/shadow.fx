#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"

struct VS_IN
{
	float3 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_Position;	// System Value라 Pixel Shader로 넘어가면, clip position이 아닌 실제 Pixel의 좌표가 됨.
	float4 clipPos : POSITION;	// clip position을 담을 데이터
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0.0f;

	output.pos = mul(float4(input.pos, 1.0f), g_matWVP);	// clip space 전환
	output.clipPos = output.pos;

	return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
	// z 값만 w로 나누어 Projection positoin으로 변환하여 반환
	return float4(input.clipPos.z / input.clipPos.w, 0.0f, 0.0f, 0.0f);
}

#endif