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
	Matrix matScale = Matrix::CreateScale(_localScale);	//크기(S)

	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x); //자전(R)(나중에 쿼터니언으로 수정할 예정)
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition); //이동(T)

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
	transformParmas.matViewInv		= Camera::S_MatView.Invert();

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParmas, sizeof(transformParmas));
}

void Transform::LookAt(const Vec3& dir)
{
	Vec3 front = dir;	// 바라보는 방향
	front.Normalize();

	Vec3 right = Vec3::Up.Cross(dir);
	if (right == Vec3::Zero)
		right = Vec3::Forward.Cross(dir);

	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();
	matrix.Right(right);
	matrix.Up(up);
	matrix.Backward(front);

	_localRotation = DecomposeRotationMatrix(matrix);
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}

Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation)
{
	Vec4 v[4];
	XMStoreFloat4(&v[0], rotation.Right());
	XMStoreFloat4(&v[1], rotation.Up());
	XMStoreFloat4(&v[2], rotation.Backward());
	XMStoreFloat4(&v[3], rotation.Translation());

	Vec3 ret;
	if (CloseEnough(v[0].z, -1.0f))
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(v[1].x, v[2].x);
		ret = Vec3{ x, y, z };
	}
	else if (CloseEnough(v[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-v[1].x, -v[2].x);
		ret = Vec3{ x, y, z };
	}
	else
	{
		float y1 = -asin(v[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
		float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

		float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
		float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			ret = Vec3{ x1, y1, z1 };
		}
		else
		{
			ret = Vec3{ x2, y2, z2 };
		}
	}

	return ret;
}

