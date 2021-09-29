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
	Matrix matScale = Matrix::CreateScale(_localscale);	//ũ��(S)

	Matrix matRotation = Matrix::CreateRotationX(_localrotation.x); //����(R)(���߿� ���ʹϾ����� ������ ����)
	matRotation *= Matrix::CreateRotationY(_localrotation.y);
	matRotation *= Matrix::CreateRotationZ(_localrotation.z);
	
	Matrix matTranslation = Matrix::CreateTranslation(_localposition); //�̵�(T)

	// S R T
	_matLocal = matScale * matRotation * matTranslation;	//ũ�� * ���� * �̵�
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();	//�θ� �ִٸ�.
	if (parent != nullptr)
	{
		_matWorld *= parent->GetLocalToWorldMatrix();	//�θ��� ����� �������.
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

