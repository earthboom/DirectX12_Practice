#pragma once

#include "Component.h"
#include "Frustum.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE,	// 원근 투영
	ORTHOGRAPHIC,	// 직교 투영
};

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void FinalUpdate() override;
	void Render();

	void SetProjectionType(PROJECTION_TYPE type) { _type = type; }
	PROJECTION_TYPE GetProjectionType() { return _type; }

	// 특정 layer를 끄고 킬 수 있는 함수.  
	// ture : 찍지 않음,  false : 찍음
	void SetCullingMaskLayerOnOff(uint8 layer, bool on)
	{
		if (on)
			_cullingMask |= (1 << layer);
		else
			_cullingMask &= ~(1 << layer);
	}

	void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }	// 아무 것도 찍지 않겠다는 의미. (모든 bit를 1로 변경)
	void SetCullingMask(uint32 mask) { _cullingMask = mask; }
	bool IsCulled(uint8 layer) { return(_cullingMask & (1 << layer)) != 0; }

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.0f;
	float _far = 1000.0f;
	float _fov = XM_PI / 4.0f;	// Field of View (시야 범위)
	float _scale = 1.0f;

	// 카메라가 여러 대일 경우를 고려하여, 각자 view, projection행렬을 가지고 있도록 함.
	Matrix _matView = {};
	Matrix _matProjection = {};

	Frustum _frustum;
	uint32 _cullingMask = 0;	// 32bit 2진수로 찍어야 할 Layer를 분류.

public:
	//Temp
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};

