#include "pch.h"
#include "CommandQueue.h"
#include "Engine.h"

//-------------------------------------
//GraphicsCommandQueue
//-------------------------------------

GraphicsCommandQueue::~GraphicsCommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// CommandQueue 생성
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	// D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
	// GPU가 하나인 시스템에선 0으로 DIRECT or BUNDLE Allocator / 초기 상태(그리기 명령 nulltpr로 지정)
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
	// CommandList는 Close / Open 상태가 있음. Open 상태에서 Command를 넣다가 Close한 다름 제출하는 개념.
	_cmdList->Close();

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));
	
	// CreateFence - CPU와 GPU의 동기화 수잔으로 쓰임.
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);	//신호등 같은 느낌?
}

// 그닥 효율적인 방법은 아님
void GraphicsCommandQueue::WaitSync()
{
	++_fenceValue;	//일에 대한 번호를 매김

	// CommandQueue에도 매긴 번호를 전달
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// 번호에 맞는 일감을 해결했을 떄, _fenceEvent 호출
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
		
		// 해당 일감이 끝날때까지 기다림.
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void GraphicsCommandQueue::RenderBegin()
{
	//초기화.
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	int8 backIndex = _swapChain->GetBackBufferIndex();

	// Buffer들이 스왑되는 용도
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 일 처리 결과물
		
	_cmdList->SetGraphicsRootSignature(GRAPHICS_ROOT_SIGNATURE.Get()); //서명.
	
	//GEngine->GetCB()->Clear(); // CB의 인덱스를 초기화.
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

	GEngine->GetGraphicsDescHeap()->Clear();

	//어떤 Heap을 사용할 것인지 지정.
	//SetDescriptorHeaps, SetGraphicsRootDescriptorTable는 함께 쓰이니 주의
	ID3D12DescriptorHeap* descHeap = GEngine->GetGraphicsDescHeap()->GetDescriptorHeap().Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);	//프레임마다 한 번씩만 해주는 것이 좋다. (매우 무겁다)

	_cmdList->ResourceBarrier(1, &barrier);

	//[[이제 여기서 수행하지 않음.]]
	//// 어떤 Buffer에 그림을 그릴 것인지를 구체적으로 정함.
	//// GPU에게 BackBuffer가 무엇인지 전해줌.
	//D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
	//_cmdList->ClearRenderTargetView(backBufferView, Colors::Black, 0, nullptr);

	////Depth Stencil Buffer의 핸들을 가져와서 OM(Out Merger)단계에서 사용.
	//D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetDepthStencilBuffer()->getDSVCpuHandle();
	//_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);

	////이후 Depth Stencil Buffer를 1로 초기화함.
	//_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void GraphicsCommandQueue::RenderEnd()
{
	int8 backIndex = _swapChain->GetBackBufferIndex();

	// BackBuffer를 현재 화면에 그려줌.
	// 현재 화면을 BackBuffer로 바꿈.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	// 더 이상의 요청을 받을 수 없도록 닫는다.
	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	// 커맨드 리스트 실행!
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	// 앞의 Buffer를 통해 화면에 그려줌.
	_swapChain->Present();

	// 프레임 명령이 완료될 때까지 기다립니다. 
	// 이러한 기다림은 비효율적이며 단순성을 위해 수행됩니다. 
	// 나중에 프레임별로 기다릴 필요가 없도록 렌더링 코드를 구성하는 방법을 보여 드리겠습니다.
	WaitSync();

	// BackBuffer index를 바꿔주어, Swap준비를 한다.
	_swapChain->SwapIndex();
}

void GraphicsCommandQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}


//-------------------------------------
//ComputeCommandQueue
//-------------------------------------

ComputeCommandQueue::~ComputeCommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device)
{
	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// CommandQueue 생성
	device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	// D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
	// GPU가 하나인 시스템에선 0으로 DIRECT or BUNDLE Allocator / 초기 상태(그리기 명령 nulltpr로 지정)
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
	// CommandList는 Close / Open 상태가 있음. Open 상태에서 Command를 넣다가 Close한 다름 제출하는 개념.
	
	// CreateFence - CPU와 GPU의 동기화 수잔으로 쓰임.
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);	//신호등 같은 느낌?
}

void ComputeCommandQueue::WaitSync()
{
	++_fenceValue;	//일에 대한 번호를 매김

	// CommandQueue에도 매긴 번호를 전달
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// 번호에 맞는 일감을 해결했을 떄, _fenceEvent 호출
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		// 해당 일감이 끝날때까지 기다림.
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void ComputeCommandQueue::FlushComputeCommandQueue()
{
	_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	COMPUTE_CMD_LIST->SetComputeRootSignature(COMPUTE_ROOT_SINGNATURE.Get());
}
