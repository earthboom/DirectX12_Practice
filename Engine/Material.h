#pragma once

#include "Object.h"

class Shader;
class Texture;

enum
{
	MATERIAL_INT_COUNT = 5,
	MATERIAL_FLOAT_COUNT = 5,
	MATERIAL_TEXTURE_COUNT = 5,
};

struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void SetFloat(uint8 index, float value) { floatParams[index] = value; }
	void SetTexOn(uint8 index, int32 value) { texOnParams[index] = value; }

	array<int32, MATERIAL_INT_COUNT> intParams;	//기존 배열과 다르게 범위 체크, vector와 유사, 고정된 크기
	array<float, MATERIAL_FLOAT_COUNT> floatParams;
	array<int32, MATERIAL_TEXTURE_COUNT> texOnParams;	//텍스처를 사용하는지 확인하는 값
};

class Material : public Object
{
public:
	Material();
	virtual ~Material();

	shared_ptr<Shader> GetShader() { return _shader; }

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.SetFloat(index, value); }
	void SetTexture(uint8 index, shared_ptr<Texture> texture) 
	{ 
		_textures[index] = texture; 
		_params.SetTexOn(index, (texture == nullptr ? 0 : 1));
	}

	void PushData();

private:
	shared_ptr<Shader>	_shader;
	MaterialParams		_params;

	array<shared_ptr<Texture>, MATERIAL_TEXTURE_COUNT>	_textures;
};

