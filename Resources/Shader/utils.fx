#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor)0.0f;

    float3 viewLightDir = (float3)0.0f;

    float diffuseRatio = 0.0f;
    float specularRatio = 0.0f;
    float distanceRatio = 1.0f; // Ư�� ���� �Ÿ��� ���� ��굵 ����� ��.

    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);// world ������ ���� ���Ⱚ�� view �������� ��ȯ ��, ����ȭ(normalize)
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal)); // ���� �ݴ� ����� �� ��ǥ���� �븻 ���� �������� �� ���� ���� cos���� ����
    }
    else if (g_light[lightIndex].lightType == 1)
    {
        // Point Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;    // ������ ��ġ�� view ��ǥ��� ��ȯ
        viewLightDir = normalize(viewPos - viewLightPos);   //���� ��ġ������ ���� ����
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        float dist = distance(viewPos, viewLightPos);   //���� ��ġ�� ������ ��ġ�� �Ÿ�
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;    // ������ ��ġ�� view ��ǥ��� ��ȯ
        viewLightDir = normalize(viewPos - viewLightPos);   // �־��� ��ġ�� ���� ���� ����
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2;    // spot light�� ���� ��

            //�־��� ��ġ�� ������ ����� ũ��
            float3 viewLightVec = viewPos - viewLightPos;
            //spot light�� �߾Ӻ��� ����
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

            float centerDist = dot(viewLightVec, viewCenterLightDir);   // �� ���� ���� cos��
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));  // ���� ��

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // �ִ� �Ÿ��� �������
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // �ִ� �þ߰��� �������
                distanceRatio = 0.f;
            else // �Ÿ��� ���� ������ ���⸦ ����
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }
            
    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));   //�ݻ籤 ����
    float3 eyeDir = normalize(viewPos); // cam -> pos�� ������, ������ �� �ݴ밡 �ʿ�
    specularRatio = saturate(dot(-eyeDir, reflectionDir));  //���� ���� cos��
    specularRatio = pow(specularRatio, 2);

    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

    return color;
}

#endif 