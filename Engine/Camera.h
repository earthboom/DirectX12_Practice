#pragma once

#include "Component.h"
#include "Frustum.h"

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

	void SetProjectionType(PROJECTION_TYPE type) { _type = type; }
	PROJECTION_TYPE GetProjectionType() { return _type; }

	void SortGameObject();
	void SortShadowObject();	// �׸��� ������ ���� ��� ��ü�� �� �� �ɷ���

	void Render_Deferred();
	void Render_Forward();
	void Render_Shadow();

	// Ư�� layer�� ���� ų �� �ִ� �Լ�.  
	// ture : ���� ����,  false : ����
	void SetCullingMaskLayerOnOff(uint8 layer, bool on)
	{
		if (on)
			_cullingMask |= (1 << layer);
		else
			_cullingMask &= ~(1 << layer);
	}

	void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }	// �ƹ� �͵� ���� �ʰڴٴ� �ǹ�. (��� bit�� 1�� ����)
	void SetCullingMask(uint32 mask) { _cullingMask = mask; }
	bool IsCulled(uint8 layer) { return(_cullingMask & (1 << layer)) != 0; }

	void SetNear(float value) { _near = value; }
	void SetFar(float value) { _far = value; }
	void SetFOV(float value) { _fov = value; }
	void SetScale(float value) { _scale = value; }
	void SetWidth(float value) { _width = value; }
	void SetHeight(float value) { _height = value; }

	Matrix& GetViewMatrix() { return _matView; }
	Matrix& GetProjectionMatrix() { return _matProjection; }

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.0f;
	float _far = 1000.0f;
	float _fov = XM_PI / 4.0f;	// Field of View (�þ� ����)
	float _scale = 1.0f;
	// ȭ�� ũ��
	float _width = 0.0f;
	float _height = 0.0f;

	// ī�޶� ���� ���� ��츦 ����Ͽ�, ���� view, projection����� ������ �ֵ��� ��.
	Matrix _matView = {};
	Matrix _matProjection = {};

	Frustum _frustum;
	uint32 _cullingMask = 0;	// 32bit 2������ ���� �� Layer�� �з�.

private:
	// �׷��� ��ü���� Deferred, Forward �����Ͽ� ������ ��
	vector<shared_ptr<GameObject>> _vecDeferred;
	vector<shared_ptr<GameObject>> _vecForward;
	vector<shared_ptr<GameObject>> _vecParticle;	// particle�� ������ ��
	vector<shared_ptr<GameObject>> _vecShadow;

public:
	//Temp
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};

