#pragma once

#include "Object.h"

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	PARTICLE_SYSTEM,
	TERRAIN,
	COLLIDER,
	ANIMATOR,
	//......
	MONO_BEHAVIOUR,	// 여러 개를 가질 수 있다.
	END
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1,
};

class GameObject;
class Transform;
class MeshRenderer;
class Animator;

class Component : public Object
{
public:
	Component(COMPONENT_TYPE type);
	virtual ~Component();

public:
	virtual void Awake() {}	// 1
	virtual void Start() {}	// 2
	virtual void Update() {}// 3
	virtual void LateUpdate() {}// 4
	virtual void FinalUpdate() {}	//모든 작업 종료 후, 마지막 처리를 담당.
	// 1->2->3->4 순으로 순환

public:
	COMPONENT_TYPE GetType() { return _type; }
	bool IsValid() { return _gameObject.expired() == false; }	// _gmaeObject 확인 함수

	shared_ptr<GameObject>		GetGameObject();
	shared_ptr<Transform>		GetTransform();
	shared_ptr<MeshRenderer>	GetMeshRenderer();
	shared_ptr<Animator>		GetAnimator();

private:
	friend class GameObject;	//GameObject에게만 접근권한을 연다.
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	COMPONENT_TYPE _type;
	weak_ptr<GameObject> _gameObject;
};

