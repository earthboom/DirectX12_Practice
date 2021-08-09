#include "pch.h"
#include "CommandQueue.h"
#include "Engine.h"

CommandQueue::~CommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// CommandQueue ����
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	// D3D12_COMMAND_LIST_TYPE_DIRECT : GPU�� ���� �����ϴ� ��� ���
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
	// GPU�� �ϳ��� �ý��ۿ��� 0���� DIRECT or BUNDLE Allocator / �ʱ� ����(�׸��� ��� nulltpr�� ����)
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
	// CommandList�� Close / Open ���°� ����. Open ���¿��� Command�� �ִٰ� Close�� �ٸ� �����ϴ� ����.
	_cmdList->Close();

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));
	
	// CreateFence - CPU�� GPU�� ����ȭ �������� ����.
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);	//��ȣ�� ���� ����?
}

// �״� ȿ������ ����� �ƴ�
void CommandQueue::WaitSync()
{
	++_fenceValue;	//�Ͽ� ���� ��ȣ�� �ű�

	// CommandQueue���� �ű� ��ȣ�� ����
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// ��ȣ�� �´� �ϰ��� �ذ����� ��, _fenceEvent ȣ��
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
		
		// �ش� �ϰ��� ���������� ��ٸ�.
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	//�ʱ�ȭ.
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	// Buffer���� ���ҵǴ� �뵵
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // ȭ�� ���
		D3D12_RESOURCE_STATE_RENDER_TARGET); // �� ó�� �����
		
	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get()); //����.
	
	//GEngine->GetCB()->Clear(); // CB�� �ε����� �ʱ�ȭ.
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

	GEngine->GetTableDescHeap()->Clear();

	//� Heap�� ����� ������ ����.
	//SetDescriptorHeaps, SetGraphicsRootDescriptorTable�� �Բ� ���̴� ����
	ID3D12DescriptorHeap* descHeap = GEngine->GetTableDescHeap()->GetDescriptorHeap().Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);	//�����Ӹ��� �� ������ ���ִ� ���� ����. (�ſ� ���̴�)

	_cmdList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	_cmdList->RSSetViewports(1, vp);
	_cmdList->RSSetScissorRects(1, rect);

	// � Buffer�� �׸��� �׸� �������� ��ü������ ����.
	// GPU���� BackBuffer�� �������� ������.
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
	_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);

	//Depth Stencil Buffer�� �ڵ��� �����ͼ� OM(Out Merger)�ܰ迡�� ���.
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetDepthStencilBuffer()->getDSVCpuHandle();
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);

	//���� Depth Stencil Buffer�� 1�� �ʱ�ȭ��.
	_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void CommandQueue::RenderEnd()
{
	// BackBuffer�� ���� ȭ�鿡 �׷���.
	// ���� ȭ���� BackBuffer�� �ٲ�.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // ���� �����
		D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���

	// �� �̻��� ��û�� ���� �� ������ �ݴ´�.
	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	// Ŀ�ǵ� ����Ʈ ����!
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	// ���� Buffer�� ���� ȭ�鿡 �׷���.
	_swapChain->Present();

	// ������ ����� �Ϸ�� ������ ��ٸ��ϴ�. 
	// �̷��� ��ٸ��� ��ȿ�����̸� �ܼ����� ���� ����˴ϴ�. 
	// ���߿� �����Ӻ��� ��ٸ� �ʿ䰡 ������ ������ �ڵ带 �����ϴ� ����� ���� �帮�ڽ��ϴ�.
	WaitSync();

	// BackBuffer index�� �ٲ��־�, Swap�غ� �Ѵ�.
	_swapChain->SwapIndex();
}

void CommandQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}
