#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"

Light::Light()
	: Component(COMPONENT_TYPE::LIGHT)
{
	// Camera Setting
	_shadowCamera = make_shared<GameObject>();
	_shadowCamera->AddComponent(make_shared<Transform>());
	_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	_lightInfo.Position = GetTransform()->GetWorldPosition();

	// 위치와 방향을 맞춰줌.
	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	_shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
	_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

	_shadowCamera->FinalUpdate();
}

void Light::Render()
{
	assert(_lightIndex >= 0);

	GetTransform()->PushData();

	if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");	// shadow Target Texture를 가져옮
		_lightMaterial->SetTexture(2, shadowTex);

		Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
		_lightMaterial->SetMatrix(0, matVP);	// ViewProjection 행렬 세팅
	}
	else
	{
		float scale = 2 * _lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	_lightMaterial->SetInt(0, _lightIndex);
	_lightMaterial->PushGraphicsData();

	_volumeMesh->Render();
}

void Light::RenderShadow()
{
	_shadowCamera->GetCamera()->SortShadowObject();	// Scene에 존재하는 모든 Object 중에 그림자 영향을 받는 Object들을 걸러줌
	_shadowCamera->GetCamera()->Render_Shadow(); // 걸러진 Object들을 실질적으로 그려주는 부분.
}

void Light::SetLightDirection(Vec3 direction)
{
	direction.Normalize();

	_lightInfo.direction = direction;

	GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type)
{
	_lightInfo.lightType = static_cast<int32>(type);

	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"DirLight");

		_shadowCamera->GetCamera()->SetScale(1.0f);
		_shadowCamera->GetCamera()->SetFar(100000.0f);
		_shadowCamera->GetCamera()->SetWidth(4096);
		_shadowCamera->GetCamera()->SetHeight(4096);
		break;
	case LIGHT_TYPE::POINT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	}
}
