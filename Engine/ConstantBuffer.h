#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	GLOBLA,		//전역 데이터
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

	void PushGraphicsData(void* buffer, uint32 size);	//글로벌 데이터(한 번만 세팅하면 됨)	
	void SetGraphicsGlobalData(void* buffer, uint32 size);	//b0 를 세팅하기 위함 (글로벌 데이터)
	void PushComputeData(void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS	GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHande(uint32 index);

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource>			_cbvBuffer;
	BYTE*							_mappedBuffer = nullptr;	//cpu 쪽에서 데이터를 밀어넣을 때, 사용하는 수
	uint32							_elementSize = 0;			//각 버퍼의 사이즈
	uint32							_elementCount = 0;			//각 버퍼의 수

	ComPtr<ID3D12DescriptorHeap>	_cbvHeap;	
	D3D12_CPU_DESCRIPTOR_HANDLE		_cpuHandleBegin = {};		//각 핸들의 시작 주소
	uint32							_handleIncrementSize = 0;	//각 핸들의 간격을 위한, 핸들의 크기

	uint32							_currentIndex = 0;			//지금 어디까지 사용했는가?

	CBV_REGISTER					_reg = {};
};


