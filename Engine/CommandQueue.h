#pragma once

class SwapChain;

//-------------------------------------
//GraphicsCommandQueue
//-------------------------------------
// 일들을 모아서 실행 요청하는 부분
class GraphicsCommandQueue
{
public:
	~GraphicsCommandQueue();

	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	void WaitSync();

	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect);
	void RenderEnd();

	//일들 모두 전달하고 리소스를 로드하고 실행하는 함수.
	void FlushResourceCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }

private:
	// Command Queue (DX12에서부터 등장)
	// - 일을 요청할 때, 하나씩 요청하면 비효율적인데서 착안
	// - [일 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 방법.
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	// 텍스처 리소스를 위한
	ComPtr<ID3D12CommandAllocator>		_resCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

	// Fence (울타리를 치는 느낌)
	// - CPU / GPU 동기화를 위한 간단한 도구.
	// - 요청된 일이 끝날 때까지 기다리는데 사용(?)
	ComPtr<ID3D12Fence>		_fence;
	uint32					_fenceValue = 0;	// CommandQueue와 연동
	HANDLE					_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>		_swapChain;
};


//-------------------------------------
//ComputeCommandQueue
//-------------------------------------

class ComputeCommandQueue
{
public:
	~ComputeCommandQueue();

	void Init(ComPtr<ID3D12Device> device);
	void WaitSync();
	void FlushComputeCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12Fence>		_fence;
	uint32					_fenceValue = 0;	// CommandQueue와 연동
	HANDLE					_fenceEvent = INVALID_HANDLE_VALUE;
};