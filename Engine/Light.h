#pragma once

#include "Component.h"

//빛의 종류
enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

//빛의 색
struct LightColor
{
	Vec4	diffuse;
	Vec4	ambient;
	Vec4	specular;
};

// 빛의 모든 정보
struct LightInfo
{
	LightColor	color;		//색
	Vec4		Position;	//위치
	Vec4		direction;	//방향
	int32		lightType;	//빛의 종류(패딩을 맞추기 위한 int32)
	float		range;		//범위
	float		angle;		//각
	int32		padding;	//size를 맞추기 위함( 패딩 관련, 실제론 의미는 없음 )
};

//shader에 넘길 정보
struct LightParams
{
	uint32		lightCount;	//빛이 몇 개?
	Vec3		padding;
	LightInfo	lights[50];	//빛의 정보 (최대 50)
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();

	virtual void FinalUpdate() override;

public:
	const LightInfo& GetLightInfo() { return _lightInfo; }
	
	void SetLightDirection(const Vec3& direction) { _lightInfo.direction = direction; }
	
	void SetDiffuse(const Vec3& diffuse) { _lightInfo.color.diffuse = diffuse; }
	void SetAmbient(const Vec3& ambient) { _lightInfo.color.ambient = ambient; }
	void SetSpecular(const Vec3& specular) { _lightInfo.color.specular = specular; }

	void SetLightType(LIGHT_TYPE type) { _lightInfo.lightType = static_cast<int32>(type); }
	void SetLightRange(float range) { _lightInfo.range = range; }
	void SetLightAngle(float angle) { _lightInfo.angle = angle; }

private:
	LightInfo _lightInfo = {};
};

