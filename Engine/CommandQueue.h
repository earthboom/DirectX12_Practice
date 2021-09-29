#pragma once

class SwapChain;

//-------------------------------------
//GraphicsCommandQueue
//-------------------------------------
// �ϵ��� ��Ƽ� ���� ��û�ϴ� �κ�
class GraphicsCommandQueue
{
public:
	~GraphicsCommandQueue();

	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	void WaitSync();

	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect);
	void RenderEnd();

	//�ϵ� ��� �����ϰ� ���ҽ��� �ε��ϰ� �����ϴ� �Լ�.
	void FlushResourceCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }

private:
	// Command Queue (DX12�������� ����)
	// - ���� ��û�� ��, �ϳ��� ��û�ϸ� ��ȿ�����ε��� ����
	// - [�� ���]�� �ϰ��� �������� ����ߴٰ� �� �濡 ��û�ϴ� ���.
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	// �ؽ�ó ���ҽ��� ����
	ComPtr<ID3D12CommandAllocator>		_resCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

	// Fence (��Ÿ���� ġ�� ����)
	// - CPU / GPU ����ȭ�� ���� ������ ����.
	// - ��û�� ���� ���� ������ ��ٸ��µ� ���(?)
	ComPtr<ID3D12Fence>		_fence;
	uint32					_fenceValue = 0;	// CommandQueue�� ����
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
	uint32					_fenceValue = 0;	// CommandQueue�� ����
	HANDLE					_fenceEvent = INVALID_HANDLE_VALUE;
};