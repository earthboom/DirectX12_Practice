#include "pch.h"
#include "Material.h"
#include "Engine.h"

Material::Material()
	: Object(OBJECT_TYPE::MATERIAL)
{
}

Material::~Material()
{
}

void Material::PushGraphicsData()
{
	//Constant Buffer View (CBV) 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&_params, sizeof(_params));
		
	//Shader Resource View (SRV) 업로드
	for (size_t i = 0; i < _textures.size(); ++i)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetGraphicsDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// Pipeline 세팅
	_shader->Update();
}

void Material::PushComputeData()
{
	// CBV upload
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&_params, sizeof(_params));

	// SRV upload
	for (size_t i = 0; i < _textures.size(); ++i)
	{
		if (_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetComputeDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// Pipeline Setting
	_shader->Update();
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	// CBV + SRV + SetPIpelineState
	PushComputeData();

	// SetDescriptorHeaps + SetComputeRootDescriptorTable
	GEngine->GetComputeDescHeap()->CommitTable();

	COMPUTE_CMD_LIST->Dispatch(x, y, z);

	GEngine->GetComputeCmdQueue()->FlushComputeCommandQueue();	// 실제적으로 실행되는 부분
}
