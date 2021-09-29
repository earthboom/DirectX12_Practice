#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "params.fx"

// u0 -> unordered access view(UAV)
// RWTexture -> Read Write Texture
RWTexture2D<float4> g_rwtex_0 : register(u0);

// GPU�� thread�� �ϳ��� �ϰ��� �ǹ���.

// Dispath(x, y, z)�� x * y * z �� ��ŭ�� Thread �׷��� �ǹ�
// numthreads( x, y, z)�� �� �ش� ��ǥ�� �׷��� x, y, z �� ��ŭ�� thread�� �ǹ�.

// SV_GroupThreadID :  �׷� ���� thread �߿� ����ǰ� �ִ� thread
// SV_GroupID : ���� thread �׷��� ��ǥ (Dispatch(x, y, z)�� (x, y, z)�� �ǹ�)
// SV_DispatchThreadID : Dispath(x, y, z) * numthreads(x, y, z) + SV_GroupID(x, y, z)�� ���� Ư���� ��
 
// thread �׷�� thread ����
// max : 1024 ( CS_5.0)
// - �ϳ��� thread �׷��� �ϳ��� ����ó���⿡�� ����
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
	if (threadIndex.y % 2 == 0)
		g_rwtex_0[threadIndex.xy] = float4(1.0f, 0.0f, 0.0f, 1.0);
	else
		g_rwtex_0[threadIndex.xy] = float4(0.0f, 1.0f, 0.0f, 1.0);
}

#endif