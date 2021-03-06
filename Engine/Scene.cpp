#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"

void Scene::Awake()
{
	// Range for문
	// 인덱스에 따른 처리가 필요없이 일괄적인 처리일 때, 용이한 for문.
	// const 레퍼런스라면 복사하지 않기 때문에 성능적으로 더욱 유리.
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Update();
	}
}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

shared_ptr<class Camera> Scene::GetMainCamera()
{
	if (_cameras.empty())
		return nullptr;

	return _cameras[0];
}

void Scene::Render()
{
	// Light 세팅
	PushLightData();

	ClearRTV();	// RenderTargetView 초기화

	RenderShadow();

	RenderDeferred();

	RenderLights();	// Lighting 관련 렌더

	RenderFinal();

	RenderForward();
}

void Scene::ClearRTV()
{
	//SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	//Shadow Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();

	//Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

	//Lighting Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void Scene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();	// Setting

	// Directional Light에 대해서만 그림자를 그림.
	for (auto& light : _lights)
	{
		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)	
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	// Deferred OMSet
	// G_BUFFER의 Render Target Texture의 데이터가 deferred.fx 의 PS_Main에서 연산됨.
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	// 메인 카메라 찾기
	// 가장 첫 카메라를 메인 카메라로 간주함.
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->SortGameObject();	// Forward, Deferred 구분	
	mainCamera->Render_Deferred();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	shared_ptr<Camera> mainCamera = _cameras[0];	// main 카메라
	Camera::S_MatView = mainCamera->GetViewMatrix();	// main 카메라의 view 행렬 
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix(); //  main 카메라의 Projection 행렬

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// 광원 그리기
	for (auto& light : _lights)
		light->Render();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void Scene::RenderFinal()
{
	// Swapchain OMSet
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();	// 취합 과정
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward()
{
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->Render_Forward();

	// 메인 카메라를 제외한 나머지 카메라는 Forward Render만 수행
	for (auto& camera : _cameras)
	{
		if (camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& light : _lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);	// 몇 번쨰 빛인지 구분
		//TODO

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBLA)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
		_cameras.emplace_back(gameObject->GetCamera());
	else if (gameObject->GetLight() != nullptr)
		_lights.emplace_back(gameObject->GetLight());

	_gameObjects.emplace_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
		if (findIt != _cameras.end())
			_cameras.erase(findIt);
	}
	else if (gameObject->GetLight() != nullptr)
	{
		auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
		if (findIt != _lights.end())
			_lights.erase(findIt);
	}
		
	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
		_gameObjects.erase(findIt);
}
