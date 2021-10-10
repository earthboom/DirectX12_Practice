#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"

MeshData::MeshData()
	: Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring& path)
{
	FBXLoader loader;	// FBX loader를 이용한 mesh 호출
	loader.LoadFbx(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); ++i)
	{
		shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i));

		GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

		// Material을 찾아서 연동
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); ++j)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
			materials.emplace_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.materials = materials;
		meshData->_meshRenders.emplace_back(info);
	}

	return meshData;
}

void MeshData::Laod(const wstring& path)
{
}

void MeshData::Save(const wstring& path)
{
}

vector<shared_ptr<GameObject>> MeshData::Instantiate()
{
	vector<shared_ptr<GameObject>> v;

	for (MeshRenderInfo& info : _meshRenders)
	{
		shared_ptr<GameObject> gameObj = make_shared<GameObject>();
		gameObj->AddComponent(make_shared<Transform>());
		gameObj->AddComponent(make_shared<MeshRenderer>());
		gameObj->GetMeshRenderer()->SetMesh(info.mesh);

		for (uint32 i = 0; i < info.materials.size(); ++i)
			gameObj->GetMeshRenderer()->SetMaterial(info.materials[i], i);

		v.emplace_back(gameObj);
	}

	return v;
}
