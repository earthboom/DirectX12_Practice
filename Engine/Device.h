#pragma once

// �η� �繫�� ����. GPU�� ������ �� �� �ִ� �繫�� ����
class Device
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI()		{ return _dxgi; }
	ComPtr<ID3D12Device> GetDevice()	{ return _device; }

private:
	// COM ( Component Object Model )
	// - DX�� ���α׷��� ��� �������� ���� ȣȯ���� �����ϰ� �ϴ� ���.
	// - COM ��ü(COM �������̽�)�� ���. ���λ����� ��������� ������.
	// - ComPtr�� ������ ����Ʈ ������.
	ComPtr<ID3D12Debug>		_debugController;
	ComPtr<IDXGIFactory>	_dxgi;				//ȭ�� ���� ���
	ComPtr<ID3D12Device>	_device;			//���� ��ü ����
};

// DX12 : MS���� �����ϴ� GPU�� �����ϰ� ���α׷����ϴµ� ���̴� ������ �׷��� API.