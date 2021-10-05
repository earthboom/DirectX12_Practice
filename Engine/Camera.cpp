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

//static 변수 정의
Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
	_width = static_cast<float>(GEngine->GetWindow().width);
	_height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	//카메라의 World 행렬을 가져와 역행렬을 구함.
	//현재 카메라의 월드 행렬의 역행렬이 바로 View 행렬.
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	// SimpleMath에서 제공되는 함수는 오른손 좌표계를 기준으로 하기 때문에, 기존 것을 사용(왼손)
	if (_type == PROJECTION_TYPE::PERSPECTIVE)	// 원근투영
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
	else // 직교투영
		_matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);
	
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
			if (_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		if (gameObject->GetMeshRenderer())
		{
			// 일반적인 forward, deferred render
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
			// 일반적인 particle
			_vecParticle.emplace_back(gameObject);
		}		
	}
}

void Camera::SortShadowObject()
{
	// 물체들을 모두 순회
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecShadow.clear();//기존 물체 제거

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)	
			continue;

		if (gameObject->IsStatic())	// Static 체크
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		_vecShadow.push_back(gameObject);
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

void Camera::Render_Shadow()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	for (auto& gameObject : _vecShadow)
	{
		gameObject->GetMeshRenderer()->RenderShadow();
	}
}

//void Camera::Render()
//{
//	S_MatView = _matView;
//	S_MatProjection = _matProjection;
//
//	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
//
//	// Layer 구분
//	const vector<shared_ptr<GameObject>>& gameobjects = scene->GetGameObjects();
//
//	for (auto& gameObject : gameobjects)
//	{
//		if (gameObject->GetMeshRenderer() == nullptr)
//			continue;
//
//		// 자신이 찍고 있는 layer인가를 구분
//		if (IsCulled(gameObject->GetLayerIndex()))
//			continue;
//
//		if (gameObject->GetCheckFrustum())	// 절두체 컬링을 허용하는 오브젝트인가?
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
