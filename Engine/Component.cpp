#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(COMPONENT_TYPE type)
	: Object(OBJECT_TYPE::COMPONENT)
	, _type(type)
{
}

Component::~Component()
{
}

shared_ptr<GameObject> Component::GetGameObject()
{
	//weak_ptr의 lock
	// shared_ptr로 변환하여, 해당 포인터에 엑세스를 지원.
	return _gameObject.lock();	
}

shared_ptr<Transform> Component::GetTransform()
{
	return _gameObject.lock()->GetTransform();
}
