#pragma once

#include "Component.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE,	// ���� ����
	ORTHOGRAPHIC,	// ���� ����
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
	float _fov = XM_PI / 4.0f;	// Field of View (�þ� ����)
	float _scale = 1.0f;

	// ī�޶� ���� ���� ��츦 ����Ͽ�, ���� view, projection����� ������ �ֵ��� ��.
	Matrix _matView = {};
	Matrix _matProjection = {};

public:
	//Temp
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};

