#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "params.fx"

// u0 -> unordered access view(UAV)
// RWTexture -> Read Write Texture
RWTexture2D<float4> g_rwtex_0 : register(u0);

// GPU의 thread는 하나의 일감을 의미함.

// Dispath(x, y, z)는 x * y * z 개 만큼의 Thread 그룹을 의미
// numthreads( x, y, z)는 그 해당 좌표의 그룹의 x, y, z 개 만큼의 thread를 의미.

// SV_GroupThreadID :  그룹 내의 thread 중에 실행되고 있는 thread
// SV_GroupID : 현재 thread 그룹의 좌표 (Dispatch(x, y, z)의 (x, y, z)를 의미)
// SV_DispatchThreadID : Dispath(x, y, z) * numthreads(x, y, z) + SV_GroupID(x, y, z)를 더한 특별한 값
 
// thread 그룹당 thread 개수
// max : 1024 ( CS_5.0)
// - 하나의 thread 그룹은 하나의 다중처리기에서 실행
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
	if (threadIndex.y % 2 == 0)
		g_rwtex_0[threadIndex.xy] = float4(1.0f, 0.0f, 0.0f, 1.0);
	else
		g_rwtex_0[threadIndex.xy] = float4(0.0f, 1.0f, 0.0f, 1.0);
}

#endif