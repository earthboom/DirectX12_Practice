#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() 
	: Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// C++ 코드에서는 이에 대해 GPU에게 생성을 요청하고, 이를 원하는 레지스터에 넘기기 위함.
	// 여기서 실제 정보를 넣지는 않음.
	_particleBuffer = make_shared<StructuredBuffer>();	// 공용 파티클에 대한 정보
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);	// _maxParticle개수 만큼 생성

	_computeSharedBuffer = make_shared<StructuredBuffer>();	
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	_material = GET_SINGLE(Resources)->Get<Material>(L"Particle");
	shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
		L"Bubbles", L"..\\Resources\\Texture\\Particle\\bubble.png");

	_material->SetTexture(0, tex);

	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::FinalUpdate()
{
	_accTime += DELTA_TIME;

	int32 add = 0;
	if (_createInterval < _accTime)	//생성 인터벌 체크
	{
		_accTime = _accTime - _createInterval;
		add = 1;
	}

	// u0, u1 레지스터에 맵핑하는 작업
	_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);

	_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));

	// Compute Shader 실행 
	_computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render()
{
	GetTransform()->PushData();

	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}
