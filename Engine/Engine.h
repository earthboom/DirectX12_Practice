#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "Texture.h"
//#include "DepthStencilBuffer.h"
#include "RenderTargetGroup.h"

class Engine
{
public:
	void Init(const WindowInfo& info);
	void Update();	

public:
	const WindowInfo& GetWindow() { return _window; }

	shared_ptr<Device> GetDevice() { return _device; }
	shared_ptr<CommandQueue> GetCmdQueue() { return _cmdQueue; }
	shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
	shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
	shared_ptr<TableDescriptorHeap> GetTableDescHeap() { return _tableDescHeap; }
	//shared_ptr<DepthStencilBuffer> GetDepthStencilBuffer() { return _depthStencilBuffer; }

	shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
	shared_ptr<RenderTargetGroup> GetRTGroup(RENDER_TARGET_GROUP_TYPE type) { return _rtGroups[static_cast<uint8>(type)]; }

public:
	void Render();
	void RenderBegin();	// CommandQueue에 여러 요청사항을 보낼 때
	void RenderEnd();	// CommandQueue의 일들을 GPU에 맡김.

	void ResizeWindow(int32 width, int32 height);	//윈도우 크기 변경

private:
	void ShowFps();
	void CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count);
	void CreateRenderTargetGroups();

private:
	// 화면 크기 관련
	WindowInfo		_window;	
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	// 임시적(편리)를 위해, 헤더에서 할당(나중에 구조 변경 때, 수정)
	shared_ptr<Device> _device = make_shared<Device>();	//GPU 접근, 요청 (가장 핵심)
	shared_ptr<CommandQueue> _cmdQueue = make_shared<CommandQueue>();	//GPU에 일을 요청할 때, 일을 한 번에 모아서 전달하는 역할
	shared_ptr<SwapChain> _swapChain = make_shared<SwapChain>();	//더블 버퍼 용, 화면을 그리고 준비하는 두 버퍼를 교체하는 역할	
	shared_ptr<RootSignature> _rootSignature = make_shared<RootSignature>();
	shared_ptr<TableDescriptorHeap> _tableDescHeap = make_shared<TableDescriptorHeap>();
	//shared_ptr<DepthStencilBuffer> _depthStencilBuffer = make_shared<DepthStencilBuffer>();

	vector<shared_ptr<ConstantBuffer>> _constantBuffers;
	array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _rtGroups;
};

