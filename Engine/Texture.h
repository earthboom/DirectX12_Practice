#pragma once

#include "Object.h"

//���� �̹����� �ҷ����� �� �Ӹ��� �ƴ϶�, RenderTarget�� �� ���۸� ����.
class Texture : public Object
{
public:
	Texture();
	virtual ~Texture();

	virtual void Load(const wstring& path);

public:
	// texture�� ó������ ����� �ִ� ���
	void Create(DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

	//������ ���۸� �̿��� Texture�� ����� ���(Front Buffer, Back Buffer)
	void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

public:
	ComPtr<ID3D12Resource> GetTex2D() { return _tex2D; }
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHandleBegin; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() { return _uavHandleBegin; }

	float GetWidth() { return static_cast<float>(_desc.Width); }
	float GetHeight() { return static_cast<float>(_desc.Height); }

private:
	ScratchImage					_image;
	D3D12_RESOURCE_DESC				_desc;
	ComPtr<ID3D12Resource>			_tex2D;

	ComPtr<ID3D12DescriptorHeap>	_srvHeap;	//�ؽ�ó�� view�� �ϳ��� ���� ���
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>	_dsvHeap;
	ComPtr<ID3D12DescriptorHeap>	_uavHeap;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandleBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE		_uavHandleBegin = {};
};

