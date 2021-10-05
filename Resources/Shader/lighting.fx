#ifndef _LIGHTING_FX_
#define _LIGHTING_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_Position; // System Value Position (예약된 것)
	float2 uv : TEXCOORD;
};

struct PS_OUT
{
	float4 diffuse : SV_Target0;
	float4 specular : SV_Target1;
};

// Directional Light
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_tex_2 : Shadow RT
// g_mat_0 : ShadowCamera VP 
// Mesh : Rectangle

VS_OUT VS_DirLight(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos * 2.0f, 1.0f);
	output.uv = input.uv;

	return output;
}

PS_OUT PS_DirLight(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	// camera를 기준으로 물체가 있는지 없는지 판단
	float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
	if (viewPos.z <= 0.0f)	// 그려줄 필요가 없을 경우
		clip(-1);

	float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;

	LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

	// 그림자 적용해야 하는지를 판별.
	if (length(color.diffuse) != 0)
	{
		matrix shadowCameraVP = g_mat_0;

		float4 worldPos = mul(float4(viewPos.xyz, 1.0f), g_matViewInv); // World 좌표계 변환
		float4 shadowClipPos = mul(worldPos, shadowCameraVP);	// 빛 기준의 카메라 Clip 좌표계 변환
		float depth = shadowClipPos.z / shadowClipPos.w;	// w 나누기로 투영좌표계로 깊이값 추출

		// x[-1 ~ 1] -> u[0 ~ 1]
		// y[1 ~ -1] -> v[0 ~ 1]
		float2 uv = shadowClipPos.xy / shadowClipPos.w;	// xy를 w로 나누어 투영좌표계 값을 추출
		uv.y = -uv.y;
		uv = uv * 0.5f + 0.5f;

		if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
		{
			float shadowDepth = g_tex_2.Sample(g_sam_0, uv).x;
			if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)	// 오차 범위를 보정을 위한 추가값
			{
				color.diffuse *= 0.5f;
				color.specular = (float4)0.0f;
			}
		}
	}

	output.diffuse = color.diffuse + color.ambient;
	output.specular = color.specular;

	return output;
} 

// Point Light
// g_int_0 : Light index
// g_tex_0 : Position_RT
// g_tex_1 : Normal RT
// g_vec2_0 : Render Target Resolution
// Mesh : Sphere

VS_OUT VS_PointLight(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(float4(input.pos, 1.0f), g_matWVP);
	output.uv = input.uv;

	return output;
}

PS_OUT PS_PointLight(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	// input.pos = SV_Position = Screen 좌표 (pixel 좌표계)
	float2 uv = float2(input.pos.x / g_vec2_0.x, input.pos.y / g_vec2_0.y);	// uv 좌표계 변환
	float3 viewPos = g_tex_0.Sample(g_sam_0, uv).xyz;
	if (viewPos.z <= 0.0f)
		clip(-1);

	int lightIndex = g_int_0;
	float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.0f), g_matView).xyz;	// view 좌표계에서의 광원 위치
	float distance = length(viewPos - viewLightPos);	// 물체와 광원의 거리
	if (distance > g_light[lightIndex].range)	// 물체 사이의 거리가 광원의 범위보다 크다면 그리지 않음.
		clip(-1);

	float viewNormal = g_tex_1.Sample(g_sam_0, uv).xyz;	// normal값 추출

	LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);	// Light Color 계산

	output.diffuse = color.diffuse + color.ambient;
	output.specular = color.specular;

	return output;
}


// Final
// 현재까지 계산한 광원 연산을 합하는 작업
// g_tex_0 : Diffuse Color Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// Mesh : Rectangle

VS_OUT VS_Final(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos * 2.0f, 1.0f);
	output.uv = input.uv;

	return output;
}

float4 PS_Final(VS_OUT input) : SV_Target
{
	float4 output = (float4)0;

	float4 lightPower = g_tex_1.Sample(g_sam_0, input.uv);
	if (lightPower.x == 0.0f && lightPower.y == 0.0f && lightPower.z == 0.0f)	// 빛의 영향이 없다고 판단되면 그리지 않음
		clip(-1);

	float4 color = g_tex_0.Sample(g_sam_0, input.uv);
	float4 specular = g_tex_2.Sample(g_sam_0, input.uv);

	output = (color * lightPower) + specular;
	return output;
}
#endif