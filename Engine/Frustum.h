#pragma once

// 절두체 평면 방향
enum PLANE_TYPE : uint8
{
	PLANE_FRONT,
	PLANE_BACK,
	PLANE_UP,
	PLANE_DOWN,
	PLANE_LEFT,
	PLANE_RIGHT,

	PLANE_END,
};

class Frustum
{
public:
	void FinalUpdate();
	bool ContainSphere(const Vec3& pos, float radius);	// 구체 판별

private:
	array<Vec4, PLANE_END> _planes;
};

