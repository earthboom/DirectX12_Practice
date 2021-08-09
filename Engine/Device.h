#pragma once

// 인력 사무소 느낌. GPU와 연락을 할 수 있는 사무소 느낌
class Device
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI()		{ return _dxgi; }
	ComPtr<ID3D12Device> GetDevice()	{ return _device; }

private:
	// COM ( Component Object Model )
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술.
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 사용자한테 숨겨짐.
	// - ComPtr은 일종의 스마트 포인터.
	ComPtr<ID3D12Debug>		_debugController;
	ComPtr<IDXGIFactory>	_dxgi;				//화면 관련 기능
	ComPtr<ID3D12Device>	_device;			//각종 객체 생성
};

// DX12 : MS에서 제공하는 GPU를 제어하고 프로그래밍하는데 쓰이는 저수준 그래픽 API.