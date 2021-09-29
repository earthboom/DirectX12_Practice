#pragma once

//-------------------------------------
// GraphicsDescriptorHeap
//-------------------------------------

class GraphicsDescriptorHeap
{
public:
	void Init(uint32 count);

	void Clear();
	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void CommitTable();	//세팅이 끝난 Descriptor Heap을 register의 table로 보내는 작업

	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:
	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	uint64 _handleSize = 0;	//각 핸들별 사이즈(간격)
	uint64 _groupSize = 0; // 각 핸들의 그룹 간격을 계산하기 위함.
	uint64 _groupCount = 0;

	uint32 _currentGroupIndex = 0;	//현재 그룹의 인덱스
};

//-------------------------------------
// ComputeDescriptorHeap
//-------------------------------------

class ComputeDescriptorHeap
{
public:
	void Init();

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg);

	void CommitTable();	//세팅이 끝난 Descriptor Heap을 register의 table로 보내는 작업

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UAV_REGISTER reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:
	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	uint64							_handleSize = 0;	//각 핸들별 사이즈(간격)
};