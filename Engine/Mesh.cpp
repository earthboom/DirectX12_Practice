#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"
#include "FBXLoader.h"

Mesh::Mesh()
	: Object(OBJECT_TYPE::MESH)
{
}

Mesh::~Mesh()
{
}

void Mesh::Create(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}

// Redner Begin과 End 사이에서 실행
void Mesh::Render(uint32 instanceCount, uint32 idx)
{
	//GRAPHICS_CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 정점들의 연결 형태
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15) vertex
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);	//인덱스 버퍼 뷰 사용

	// 1) Buffer에다가 데이터 세팅
	// 2) Table Descriptor Heap에다가 CBV 전달
	// 3) 모든 세팅이 끝나면, Table Decriptor Heap을 커밋

	// GPU register에 있는 Table로 전송
	GEngine->GetGraphicsDescHeap()->CommitTable();

	// Input Assembler(IA) 단계에 Vertex, Index정보를 전달하는 단계
	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);	// 예약하는 것에 가깝고, RenderEnd에서 실행, Vertex만 이용

	// Instancing 기술 활용 ( Draw 콜을 줄임 )
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);	//index 를 이용해 그리는 방식. (훨씬 효율적!)
}

void Mesh::Render(shared_ptr<class InstancingBuffer>& buffer, uint32 idx)
{
	D3D12_VERTEX_BUFFER_VIEW bufferView[] = { _vertexBufferView, buffer->GetBufferView() };
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferView);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, buffer->GetCount(), 0, 0, 0);
}

shared_ptr<Mesh> Mesh::CreateFromFBX(const FbxMeshInfo* meshInfo)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->CreateVertexBuffer(meshInfo->vertices);

	for (const vector<uint32>& buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			// FBX 파일이 이상하다는 의미.
			// IndexBuffer가 없으면 에러가 발생하기 때문에 임시적으로 처리.
			vector<uint32> defaultBuffer{ 0 };
			mesh->CreateIndexBuffer(defaultBuffer);
		}
		else
		{
			mesh->CreateIndexBuffer(buffer);
		}
	}

	return mesh;
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());	//render할 때 필요하기 때문에, vertex 수를 저장.
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// D3D12_HEAP_TYPE_UPLOAD : 실시간으로 Data를 전달하는 용도
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// GPU 쪽에 공간을 할당 받는 요청 함수
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));
	// _vertexBuffer : GPU 안의 공간을 가리킴.

	void* vertexDataBuffer = nullptr;	// 삼각형 데이터를 정점 버퍼에 복사
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer); // 뚜껑을 연다고 표현.
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr); // 뚜껑을 닫는다.

	// Initialize the vertex buffer view.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	uint32 indexCount = static_cast<uint32>(buffer.size());	//render할 때 필요하기 때문에, vertex 수를 저장.
	uint32 bufferSize = indexCount * sizeof(uint32);

	// D3D12_HEAP_TYPE_UPLOAD : 실시간으로 Data를 전달하는 용도
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	// GPU 쪽에 공간을 할당 받는 요청 함수
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer));
	// _indexBuffer : GPU 안의 공간을 가리킴.

	void* indexDataBuffer = nullptr;	// 삼각형 데이터를 인덱스 버퍼에 복사
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	indexBuffer->Map(0, &readRange, &indexDataBuffer); // 뚜껑을 연다고 표현.
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr); // 뚜껑을 닫는다.

	// Initialize the vertex buffer view.
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // uint32를 사용하고 있어서
	indexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기

	IndexBufferInfo info =
	{
		indexBuffer,
		indexBufferView,
		DXGI_FORMAT_R32_UINT,
		indexCount
	};

	_vecIndexInfo.emplace_back(info);
}
