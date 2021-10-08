#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider()
	: BaseCollider(ColliderType::Sphere)
{
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::FinalUpdate()
{
	_boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	_boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

// rayOrigin : 쏘는 광선의 시작점
// rayDir : 광선의 방향
// distance : 거리
bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}
