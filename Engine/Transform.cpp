#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{
}

Transform::~Transform()
{
}

void Transform::FinalUpdate()
{
	Matrix matScale = Matrix::CreateScale(_localscale);	//크기(S)

	Matrix matRotation = Matrix::CreateRotationX(_localrotation.x); //자전(R)(나중에 쿼터니언으로 수정할 예정)
	matRotation *= Matrix::CreateRotationY(_localrotation.y);
	matRotation *= Matrix::CreateRotationZ(_localrotation.z);
	
	Matrix matTranslation = Matrix::CreateTranslation(_localposition); //이동(T)

	// S R T
	_matLocal = matScale * matRotation * matTranslation;	//크기 * 자전 * 이동
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();	//부모가 있다면.
	if (parent != nullptr)
	{
		_matWorld *= parent->GetLocalToWorldMatrix();	//부모의 행렬을 계산해줌.
	}
}

void Transform::PushData()
{
	//World View Project (W V P)
	//  TO DO
	TransformParams transformParmas = {};
	transformParmas.matWorld		= _matWorld;
	transformParmas.matView			= Camera::S_MatView;
	transformParmas.matProjection	= Camera::S_MatProjection;
	transformParmas.matWV			= _matWorld * Camera::S_MatView;
	transformParmas.matWVP			= _matWorld * Camera::S_MatView * Camera::S_MatProjection;

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParmas, sizeof(transformParmas));
}

