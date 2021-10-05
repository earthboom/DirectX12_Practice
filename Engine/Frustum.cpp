#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

void Frustum::FinalUpdate()
{
	Matrix matViewInv = Camera::S_MatView.Invert();	// view 행렬 역행렬
	Matrix matProjectionInv = Camera::S_MatProjection.Invert();	// projection 행렬 역행렬
	Matrix matInv = matProjectionInv * matViewInv;	//world까지 내려옮

	// projection space를 기준으로 한, 투영 좌표계를 world 좌표로 만듦
	vector<Vec3> worldPos =
	{
		::XMVector3TransformCoord(Vec3(-1.0f, 1.0f,  0.0f), matInv),
		::XMVector3TransformCoord(Vec3(1.0f,  1.0f,  0.0f), matInv),
		::XMVector3TransformCoord(Vec3(1.0f,  -1.0f, 0.0f), matInv),
		::XMVector3TransformCoord(Vec3(-1.0f, -1.0f, 0.0f), matInv),
		::XMVector3TransformCoord(Vec3(-1.0f, 1.0f,  1.0f), matInv),
		::XMVector3TransformCoord(Vec3(1.0f,  1.0f,  1.0f), matInv),
		::XMVector3TransformCoord(Vec3(1.0f,  -1.0f, 1.0f), matInv),
		::XMVector3TransformCoord(Vec3(-1.0f, -1.0f, 1.0f), matInv),
	};

	_planes[PLANE_FRONT]	= ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]);	//CW
	_planes[PLANE_BACK]		= ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]);	//CCW
	_planes[PLANE_UP]		= ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]);	//CW
	_planes[PLANE_DOWN]		= ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]);	//CCW
	_planes[PLANE_LEFT]		= ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]);	//CW
	_planes[PLANE_RIGHT]	= ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]);	//CCW
}

bool Frustum::ContainsSphere(const Vec3& pos, float radius)
{
	// 기존에 쓰이는 BoundingFrusum 구조체 (키워드)

	for(const Vec4& plane : _planes)
	{
		// n = (a, b, c)
		Vec3 normal = Vec3(plane.x, plane.y, plane.z);

		// ax + by + cz + d > radius
		if (normal.Dot(pos) + plane.w > radius)
			return false;
	}

	return true;
}
