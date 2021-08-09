#pragma once

#include "Component.h"

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	virtual void FinalUpdate() override;
	void PushData();

public:
	const Vec3& GetLocalPosition() { return _localposition; }
	const Vec3& GetLocalRotation() { return _localrotation; }
	const Vec3& GetLocalScale() { return _localscale; }

	const Matrix& GetLocalToWorldMatrix() { return _matWorld; }	// Local -> World 변환 행렬
	Vec3 GetWorldPosition() { return _matWorld.Translation(); }	// World 좌표

	// World 기준
	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }	// 바라보는 방향 (look vector)

	void SetLocalPosition(const Vec3& position) { _localposition = position; }
	void SetLocalRotation(const Vec3& rotation) { _localrotation = rotation; }
	void SetLocalScale(const Vec3& scale) { _localscale = scale; }
	
public:
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	weak_ptr<Transform> GetParent() { return _parent; }

private:
	// Parent 기준
	Vec3 _localposition = {};
	Vec3 _localrotation = {};
	Vec3 _localscale = { 1.0f, 1.0f, 1.0f };

	Matrix _matLocal = {};
	Matrix _matWorld = {};

	weak_ptr<Transform> _parent;
};

