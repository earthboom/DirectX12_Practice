#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"

//static ���� ����
Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	//ī�޶��� World ����� ������ ������� ����.
	//���� ī�޶��� ���� ����� ������� �ٷ� View ���.
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	// SimpleMath���� �����Ǵ� �Լ��� ������ ��ǥ�踦 �������� �ϱ� ������, ���� ���� ���(�޼�)
	if (_type == PROJECTION_TYPE::PERSPECTIVE)	// ��������
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
	else // ��������
		_matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);
	
	_frustum.FinalUpdate();
}

void Camera::Render()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// Layer ����
	const vector<shared_ptr<GameObject>>& gameobjects = scene->GetGameObjects();

	for (auto& gameObject : gameobjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		// �ڽ��� ��� �ִ� layer�ΰ��� ����
		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())	// ����ü �ø��� ����ϴ� ������Ʈ�ΰ�?
		{
			if (_frustum.ContainSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		gameObject->GetMeshRenderer()->Render();
	}
}
