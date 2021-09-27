#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"

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

void Scene::Render()
{
	// Light 세팅
	PushLightData();

	//SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	//Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

	// Camera를 찾아서 Render함.
	for (auto& gameObject : _gameObjects)
	{
		if (gameObject->GetCamera() == nullptr)
			continue;

		// Forward, Deferred 구분
		gameObject->GetCamera()->SortGameObject();

		// Deferred OMSet
		// G_BUFFER의 Render Target Texture의 데이터가 deferred.fx 의 PS_Main에서 연산됨.
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();
		gameObject->GetCamera()->Render_Deferred();

		// Light OMSet

		// Swapchain OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);
		gameObject->GetCamera()->Render_Forward();
	}
}

void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& gameObject : _gameObjects)
	{
		if(gameObject->GetLight() == nullptr)
			continue;

		const LightInfo& lightInfo = gameObject->GetLight()->GetLightInfo();

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBLA)->SetGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	_gameObjects.emplace_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
	{
		_gameObjects.erase(findIt);
	}
}
