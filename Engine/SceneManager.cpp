#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Resources.h"

#include "TestCameraScript.h"

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

//TEMP
void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::LoadScene(wstring sceneName)
{
	//기존 Scene 정리
	//파일에서 Scene 정보 로드
	
	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
	_activeScene->FinalUpdate();
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();
	
#pragma region Camera
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near = 1, Far = 1000, FOV = 45도
	camera->AddComponent(make_shared<TestCameraScript>());	//카메라 이동코드를 Component로 빼서 작업.
	
	camera->GetTransform()->SetLocalPosition(Vec3(0.0f, 100.0f, 0.0f));
	scene->AddGameObject(camera);
#pragma endregion

#pragma region Sphere
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.0f, 100.0f, 100.0f));
		sphere->GetTransform()->SetLocalPosition(Vec3(0.0f, 100.0f, 200.0f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			shared_ptr<Texture> texture = make_shared<Texture>();
			shader->Init(L"..\\Resources\\Shader\\default.hlsli");
			texture->Init(L"..\\Resources\\Texture\\veigar.jpg");
			
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		sphere->AddComponent(meshRenderer);
		scene->AddGameObject(sphere);
	}
#pragma endregion

#pragma region Cube
	{
		shared_ptr<GameObject> cube = make_shared<GameObject>();
		cube->AddComponent(make_shared<Transform>());
		cube->GetTransform()->SetLocalScale(Vec3(100.0f, 100.0f, 100.0f));
		cube->GetTransform()->SetLocalPosition(Vec3(150.0f, 100.0f, 200.0f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(cubeMesh);
		}
		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			shared_ptr<Texture> texture = make_shared<Texture>();
			shader->Init(L"..\\Resources\\Shader\\default.hlsli");
			texture->Init(L"..\\Resources\\Texture\\veigar.jpg");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		cube->AddComponent(meshRenderer);
		scene->AddGameObject(cube);
	}
#pragma endregion

	return scene;
}
