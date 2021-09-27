#pragma once

//교환 사슬
// "일 처리 과정"
//	- 현재 게임 세상에 있는 상황 묘사
//	- 어떤 공식으로 어떻게 계산할지 던져줌
//	- GPU가 열심히 계산 (일 처리)
//	- 결과물을 받아 화면에 그림

// "일 처리 결과물"을 어디서 받는가?
//	- 어떤 종이(Buffer)에 그려서 건내달라고 부탁.
//	- 특수 종이를 만듦->처음에 건냄->결과물을 해당 종이에 받음.
//	- 우리 화면에 특수 종이("일 처리 결과물") 출력.


//	- 그런데 화면에 현재 결과물을 출력하는 와중에, 다음 화면도 일 처리를 맡겨야 함.
//	- 현재 화면 결과물은 이미 화면 출력에 사용 중
//	- 특수 종이를 2개 만들어, 하나는 현재 화면을 그리고, 하나는 일 처리를 맡기고
//	- Double Buffering !

// - [ 1 ], [ 2 ] (전면 버퍼, 후면 버퍼)
// 현재화면 [ 1 ]  <-> GPU 작업중 [ 2 ] (BackBuffer)

class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();
	
	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	uint32 GetBackBufferIndex() { return _backBufferIndex; }
	//ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _rtvBuffer[index]; }

	//ComPtr<ID3D12Resource> GetBackRTVBuffer() { return _rtvBuffer[_backBufferIndex]; }
	//D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIndex]; }

private:
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	//void CreateRTV(ComPtr<ID3D12Device> device);

private:
	ComPtr<IDXGISwapChain>	_swapChain;

	//리소스에 대해 전체적으로 서술
	// { {view}, {view} } -> 각각의 view는 리소스를 설명.
	//ComPtr<ID3D12Resource>			_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];	//특수 종이에 해당.
	//ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	uint32	_backBufferIndex = 0;	//현재의 back buffer를 알려줌.
};

// DescriptorHeap (View라고도 함)
// "기안서"와 유사
// "일 처리"를 맡길 떄, 여러 정보들을 같이 넘겨줘야 함.
// 아무 형태로 요청하면, 알아 들을 수가 없음.
// 각 리소스들을 어떤 용도로 사용하는지, 명확히 서술하여 넘겨주는 용도