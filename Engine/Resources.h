#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Resources
{
	DECLARE_SINGLE(Resources);

public:
	void Init();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	OBJECT_TYPE GetObjectType();

	shared_ptr<Mesh> LoadRectangleMesh();
	shared_ptr<Mesh> LoadCubeMesh();
	shared_ptr<Mesh> LoadSphereMesh();

	shared_ptr<Texture> CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE, Vec4 clearColor = Vec4());

	shared_ptr<Texture> CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D);

private:
	void CreateDefulatShader();

private:
	using KeyObjMap = std::map<wstring/*key*/, shared_ptr<Object>>;
	array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;
};

// template 함수는 같은 헤더 파일에 만들어줌.
// inl파일을 따로 만들어 함수를 정의하는 경우도 있음.
template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& KeyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = KeyObjMap.find(key);
	if (findIt != KeyObjMap.end())	//존재한다면, 반환.
		return static_pointer_cast<T>(findIt->second);

	//존재하지 않는다면, 로드하여 저장하고 반환.
	shared_ptr<T> object = make_shared<T>();
	object->Load(path);
	KeyObjMap[key] = object;

	return object;
}

template<typename T>
inline bool Resources::Add(const wstring& key, shared_ptr<T> object)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& KeyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = KeyObjMap.find(key);
	if (findIt != KeyObjMap.end())
		return false;

	KeyObjMap[key] = object;

	return true;
}

template<typename T>
inline shared_ptr<T> Resources::Get(const wstring& key)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& KeyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = KeyObjMap.find(key);
	if (findIt != KeyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
inline OBJECT_TYPE Resources::GetObjectType()
{
	// is_same_v<class T, class T> (C++ 11)
	// 컴파일 타임에 계산, class type을 비교
	if (std::is_same_v<T, GameObject>)
		return OBJECT_TYPE::GAMEOBJECT;
	else if (std::is_same_v<T, Material>)
		return OBJECT_TYPE::MATERIAL;
	else if (std::is_same_v<T, Mesh>)
		return OBJECT_TYPE::GAMEOBJECT;
	else if (std::is_same_v<T, Shader>)
		return OBJECT_TYPE::SHADER;
	else if (std::is_same_v<T, Texture>)
		return OBJECT_TYPE::TEXTURE;
	else if (std::is_convertible_v<T, Component>)
		return OBJECT_TYPE::COMPONENT;
	else
		return OBJECT_TYPE::NONE;
}
