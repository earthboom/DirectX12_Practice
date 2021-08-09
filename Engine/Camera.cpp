#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"

//static 변수 정의
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
	//카메라의 World 행렬을 가져와 역행렬을 구함.
	//현재 카메라의 월드 행렬의 역행렬이 바로 View 행렬.
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	// SimpleMath에서 제공되는 함수는 오른손 좌표계를 기준으로 하기 때문에, 기존 것을 사용(왼손)
	if (_type == PROJECTION_TYPE::PERSPECTIVE)	// 원근투영
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
	else // 직교투영
		_matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

	S_MatView = _matView;
	S_MatProjection = _matProjection;
}

void Camera::Render()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// Layer 구분
	const vector<shared_ptr<GameObject>>& gameobjects = scene->GetGameObjects();

	for (auto& gameObject : gameobjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		gameObject->GetMeshRenderer()->Render();
	}
}
