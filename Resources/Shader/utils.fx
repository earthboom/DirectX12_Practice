#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor)0.0f;

    float3 viewLightDir = (float3)0.0f;

    float diffuseRatio = 0.0f;
    float specularRatio = 0.0f;
    float distanceRatio = 1.0f; // 특정 빛은 거리에 따른 계산도 해줘야 함.

    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);// world 기준의 빛의 방향값을 view 기준으로 변환 후, 정규화(normalize)
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal)); // 빛의 반대 방향과 뷰 좌표계의 노말 값의 내적값은 그 사이 각의 cos값이 나옮
    }
    else if (g_light[lightIndex].lightType == 1)
    {
        // Point Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;    // 광원의 위치를 view 좌표계로 변환
        viewLightDir = normalize(viewPos - viewLightPos);   //기준 위치에서의 빛의 방향
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        float dist = distance(viewPos, viewLightPos);   //기준 위치와 광원의 위치의 거리
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;    // 광원의 위치를 view 좌표계로 변환
        viewLightDir = normalize(viewPos - viewLightPos);   // 주어진 위치로 오는 빛의 방향
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2;    // spot light의 각의 반

            //주어진 위치와 광원의 방향과 크기
            float3 viewLightVec = viewPos - viewLightPos;
            //spot light의 중앙빛의 방향
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

            float centerDist = dot(viewLightVec, viewCenterLightDir);   // 둘 사이 각의 cos값
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));  // 사이 각

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지
                distanceRatio = 0.f;
            else // 거리에 따라 적절히 세기를 조절
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }
            
    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));   //반사광 방향
    float3 eyeDir = normalize(viewPos); // cam -> pos가 구해짐, 본래는 그 반대가 필요
    specularRatio = saturate(dot(-eyeDir, reflectionDir));  //사이 각의 cos값
    specularRatio = pow(specularRatio, 2);

    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

    return color;
}

#endif 