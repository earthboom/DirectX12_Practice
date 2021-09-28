#pragma once

#include "Texture.h"

//Redner Target == Texture

enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN,	// Back Buffer, Front Buffer 
	G_BUFFER,	// Position, Normal, Color (G Buffer는 Geometry Buffer를 의미)
	LIGHTING,	// Diffuse Light, Specular Light
	END,
};

enum
{
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};

struct RenderTarget
{
	shared_ptr<Texture> target;	// 그릴 대상이 될 Texture
	float clearColor[4];		// 초기값.
};

class RenderTargetGroup
{
public:
	// dsTexture(Depth Stencil)은 따로 받아 줌.
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture);
	
	void OMSetRenderTargets(uint32 count, uint32 offset);
	void OMSetRenderTargets();

	void ClearRenderTargetView(uint32 index);
	void ClearRenderTargetView();

	shared_ptr<Texture> GetRTTexture(uint32 index) { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() { return _dsTexture; }

	void WaitTargetToResource();
	void WaitResourceToTarget();

private:
	RENDER_TARGET_GROUP_TYPE		_groupType;
	vector<RenderTarget>			_rtVec;
	uint32							_rtCount;
	shared_ptr<Texture>				_dsTexture;
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;

private:
	uint32							_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE		_dsvHeapBegin;

private:
	D3D12_RESOURCE_BARRIER			_targetToResource[8];	// RenderTarget -> Resource
	D3D12_RESOURCE_BARRIER			_resourceToTarget[8];	// Resource -> RenderTarget
};

