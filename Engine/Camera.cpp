#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"

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

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecDeferred.clear();
	_vecForward.clear();
	_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (_frustum.ContainSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		if (gameObject->GetMeshRenderer())
		{
			// �Ϲ����� forward, deferred render
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.emplace_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				_vecForward.emplace_back(gameObject);
				break;
			}
		}
		else
		{
			// �Ϲ����� particle
			_vecParticle.emplace_back(gameObject);
		}		
	}
}

void Camera::Render_Deferred()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecDeferred);
}

void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecForward);

	// particle render
	for (auto& gameObject : _vecParticle)
		gameObject->GetParticleSystem()->Render();
}

//void Camera::Render()
//{
//	S_MatView = _matView;
//	S_MatProjection = _matProjection;
//
//	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
//
//	// Layer ����
//	const vector<shared_ptr<GameObject>>& gameobjects = scene->GetGameObjects();
//
//	for (auto& gameObject : gameobjects)
//	{
//		if (gameObject->GetMeshRenderer() == nullptr)
//			continue;
//
//		// �ڽ��� ��� �ִ� layer�ΰ��� ����
//		if (IsCulled(gameObject->GetLayerIndex()))
//			continue;
//
//		if (gameObject->GetCheckFrustum())	// ����ü �ø��� ����ϴ� ������Ʈ�ΰ�?
//		{
//			if (_frustum.ContainSphere(
//				gameObject->GetTransform()->GetWorldPosition(),
//				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
//			{
//				continue;
//			}
//		}
//
//		gameObject->GetMeshRenderer()->Render();
//	}
//}
