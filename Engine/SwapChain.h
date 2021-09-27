#pragma once

//��ȯ �罽
// "�� ó�� ����"
//	- ���� ���� ���� �ִ� ��Ȳ ����
//	- � �������� ��� ������� ������
//	- GPU�� ������ ��� (�� ó��)
//	- ������� �޾� ȭ�鿡 �׸�

// "�� ó�� �����"�� ��� �޴°�?
//	- � ����(Buffer)�� �׷��� �ǳ��޶�� ��Ź.
//	- Ư�� ���̸� ����->ó���� �ǳ�->������� �ش� ���̿� ����.
//	- �츮 ȭ�鿡 Ư�� ����("�� ó�� �����") ���.


//	- �׷��� ȭ�鿡 ���� ������� ����ϴ� ���߿�, ���� ȭ�鵵 �� ó���� �ðܾ� ��.
//	- ���� ȭ�� ������� �̹� ȭ�� ��¿� ��� ��
//	- Ư�� ���̸� 2�� �����, �ϳ��� ���� ȭ���� �׸���, �ϳ��� �� ó���� �ñ��
//	- Double Buffering !

// - [ 1 ], [ 2 ] (���� ����, �ĸ� ����)
// ����ȭ�� [ 1 ]  <-> GPU �۾��� [ 2 ] (BackBuffer)

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

	//���ҽ��� ���� ��ü������ ����
	// { {view}, {view} } -> ������ view�� ���ҽ��� ����.
	//ComPtr<ID3D12Resource>			_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];	//Ư�� ���̿� �ش�.
	//ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	uint32	_backBufferIndex = 0;	//������ back buffer�� �˷���.
};

// DescriptorHeap (View��� ��)
// "��ȼ�"�� ����
// "�� ó��"�� �ñ� ��, ���� �������� ���� �Ѱ���� ��.
// �ƹ� ���·� ��û�ϸ�, �˾� ���� ���� ����.
// �� ���ҽ����� � �뵵�� ����ϴ���, ��Ȯ�� �����Ͽ� �Ѱ��ִ� �뵵