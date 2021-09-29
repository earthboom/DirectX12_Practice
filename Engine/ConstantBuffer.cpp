#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"

ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
	if (_cbvBuffer)
	{
		if (_cbvBuffer != nullptr)
			_cbvBuffer->Unmap(0, nullptr);

		_cbvBuffer = nullptr;
	}
}

void ConstantBuffer::Init(CBV_REGISTER reg, uint32 size, uint32 count)
{
	_reg = reg;

	// 상수 버퍼는 256 바이트 배수로 만들어야 한다
	// 0 256 512 768
	_elementSize = (size + 255) & ~255;
	_elementCount = count;

	CreateBuffer();
	CreateView();
}

void ConstantBuffer::CreateBuffer()
{
	uint32 bufferSize = _elementSize * _elementCount;
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer));

	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
	// 우리는 자원을 다 쓸 때까지 지도를 풀 필요가 없음. 
	// 그러나 GPU가 리소스를 사용하는 동안에는 리소스에 기록해선 않됨. 
	// 따라서 동기화 기술을 사용해야 함.
}

void ConstantBuffer::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = _elementCount;	// 버퍼의 수만큼 생성
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//Shader로 쓰지 않도록 주의
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));	//Descariptor Heap 생성

	//핸들의 시작 주소
	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
	//각 컴퓨터 마다 다를 수 있기 떄문에, 반드시 밑의 함수로 Handle 사이즈를 구해야 함.
	_handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHande(i);	//핸들의 위치를 받음.

		//Constant Buffer View의 설정
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
		cbvDesc.SizeInBytes = _elementSize;

		DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);
	}
}

void ConstantBuffer::Clear()
{
	_currentIndex = 0;
}

//Data를 넣은 핸들의 위치를 반환해준다.
void ConstantBuffer::PushGraphicsData(void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount); //현재의 인덱스가 할당해준 버퍼 수를 넘어갈 경우
	assert(_elementSize == ((size + 255) & ~255));

	// 해당 버퍼 위치에 복사
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	//D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex); //GPU의 버퍼 가상 주소를 받음.
	//CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);	//레지스터에 참조할 버퍼 주소값을 전달

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHande(_currentIndex);

	GEngine->GetGraphicsDescHeap()->SetCBV(cpuHandle, _reg);

	++_currentIndex;
}

void ConstantBuffer::SetGraphicsGlobalData(void* buffer, uint32 size)
{
	assert(_elementSize == ((size + 255) & ~255));
	::memcpy(&_mappedBuffer[0], buffer, size);	// index 0 의 buffer data를 저장
	GRAPHICS_CMD_LIST->SetGraphicsRootConstantBufferView(0, GetGpuVirtualAddress(0));
}

void ConstantBuffer::PushComputeData(void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount); //현재의 인덱스가 할당해준 버퍼 수를 넘어갈 경우
	assert(_elementSize == ((size + 255) & ~255));

	// 해당 버퍼 위치에 복사
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHande(_currentIndex);
	GEngine->GetComputeDescHeap()->SetCBV(cpuHandle, _reg);

	++_currentIndex;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBuffer->GetGPUVirtualAddress(); //버퍼의 시작 주소를 받음.
	objCBAddress += index * _elementSize; // 현재의 인덱스를 곱하여 현재의 주소값을 계산.
	return objCBAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHande(uint32 index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);
}
