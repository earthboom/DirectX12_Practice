#pragma once

#include "Component.h"

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

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.0f;
	float _far = 1000.0f;
	float _fov = XM_PI / 4.0f;	// Field of View (시야 범위)
	float _scale = 1.0f;

	// 카메라가 여러 대일 경우를 고려하여, 각자 view, projection행렬을 가지고 있도록 함.
	Matrix _matView = {};
	Matrix _matProjection = {};

public:
	//Temp
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};

