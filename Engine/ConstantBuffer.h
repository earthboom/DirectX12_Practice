#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	GLOBLA,		//���� ������
	TRANSFORM,
	MATERIAL,
	END
};

enum
{
	CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END),
};

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(CBV_REGISTER reg, uint32 size, uint32 count);
	
	void Clear();

	void PushGraphicsData(void* buffer, uint32 size);	//�۷ι� ������(�� ���� �����ϸ� ��)	
	void SetGraphicsGlobalData(void* buffer, uint32 size);	//b0 �� �����ϱ� ���� (�۷ι� ������)
	void PushComputeData(void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS	GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHande(uint32 index);

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource>			_cbvBuffer;
	BYTE*							_mappedBuffer = nullptr;	//cpu �ʿ��� �����͸� �о���� ��, ����ϴ� ��
	uint32							_elementSize = 0;			//�� ������ ������
	uint32							_elementCount = 0;			//�� ������ ��

	ComPtr<ID3D12DescriptorHeap>	_cbvHeap;	
	D3D12_CPU_DESCRIPTOR_HANDLE		_cpuHandleBegin = {};		//�� �ڵ��� ���� �ּ�
	uint32							_handleIncrementSize = 0;	//�� �ڵ��� ������ ����, �ڵ��� ũ��

	uint32							_currentIndex = 0;			//���� ������ ����ߴ°�?

	CBV_REGISTER					_reg = {};
};


