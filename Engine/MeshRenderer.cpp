#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "InstancingBuffer.h"
#include "Resources.h"
#include "Animator.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESH_RENDERER)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetMaterial(shared_ptr<Material> material, uint32 idx)
{
	if (_materials.size() <= static_cast<size_t>(idx))
		_materials.resize(static_cast<size_t>(idx + 1));

	_materials[idx] = material;
}

void MeshRenderer::Render()
{
	for (uint32 i = 0; i < _materials.size(); ++i)
	{
		shared_ptr<Material>& mat = _materials[i];

		if (mat == nullptr || mat->GetShader() == nullptr)
			continue;

		GetTransform()->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
			mat->SetInt(1, 1);
		}

		mat->PushGraphicsData();

		_mesh->Render(1, i);
	}
}

void MeshRenderer::Render(shared_ptr<class InstancingBuffer>& buffer)
{
	for (uint32 i = 0; i < _materials.size(); ++i)
	{
		shared_ptr<Material>& mat = _materials[i];

		if (mat == nullptr || mat->GetShader() == nullptr)
			continue;

		buffer->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
			mat->SetInt(1, 1);
		}

		mat->PushGraphicsData();

		_mesh->Render(buffer, i);
	}
}

void MeshRenderer::RenderShadow()
{
	GetTransform()->PushData();
	GET_SINGLE(Resources)->Get<Material>(L"Shadow")->PushGraphicsData();	// Shadow ¼ÎÀÌ´õ È£Ãâ
	_mesh->Render();
}

uint64 MeshRenderer::GetInstanceID()
{
	if (_mesh == nullptr || _materials.empty())
		return 0;

	// uint64 id = (_mesh->GetID() << 32 | _material->Get()0
	InstanceID instanceID{ _mesh->GetID(), _materials[0]->GetID() };
	return instanceID.id;
}
