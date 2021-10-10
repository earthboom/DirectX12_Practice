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

// Redner Begin�� End ���̿��� ����
void Mesh::Render(uint32 instanceCount, uint32 idx)
{
	//GRAPHICS_CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �������� ���� ����
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15) vertex
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);	//�ε��� ���� �� ���

	// 1) Buffer���ٰ� ������ ����
	// 2) Table Descriptor Heap���ٰ� CBV ����
	// 3) ��� ������ ������, Table Decriptor Heap�� Ŀ��

	// GPU register�� �ִ� Table�� ����
	GEngine->GetGraphicsDescHeap()->CommitTable();

	// Input Assembler(IA) �ܰ迡 Vertex, Index������ �����ϴ� �ܰ�
	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);	// �����ϴ� �Ϳ� ������, RenderEnd���� ����, Vertex�� �̿�

	// Instancing ��� Ȱ�� ( Draw ���� ���� )
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);	//index �� �̿��� �׸��� ���. (�ξ� ȿ����!)
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
			// FBX ������ �̻��ϴٴ� �ǹ�.
			// IndexBuffer�� ������ ������ �߻��ϱ� ������ �ӽ������� ó��.
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
	_vertexCount = static_cast<uint32>(buffer.size());	//render�� �� �ʿ��ϱ� ������, vertex ���� ����.
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// D3D12_HEAP_TYPE_UPLOAD : �ǽð����� Data�� �����ϴ� �뵵
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// GPU �ʿ� ������ �Ҵ� �޴� ��û �Լ�
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));
	// _vertexBuffer : GPU ���� ������ ����Ŵ.

	void* vertexDataBuffer = nullptr;	// �ﰢ�� �����͸� ���� ���ۿ� ����
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer); // �Ѳ��� ���ٰ� ǥ��.
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr); // �Ѳ��� �ݴ´�.

	// Initialize the vertex buffer view.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // ���� 1�� ũ��
	_vertexBufferView.SizeInBytes = bufferSize; // ������ ũ��
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	uint32 indexCount = static_cast<uint32>(buffer.size());	//render�� �� �ʿ��ϱ� ������, vertex ���� ����.
	uint32 bufferSize = indexCount * sizeof(uint32);

	// D3D12_HEAP_TYPE_UPLOAD : �ǽð����� Data�� �����ϴ� �뵵
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	// GPU �ʿ� ������ �Ҵ� �޴� ��û �Լ�
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer));
	// _indexBuffer : GPU ���� ������ ����Ŵ.

	void* indexDataBuffer = nullptr;	// �ﰢ�� �����͸� �ε��� ���ۿ� ����
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	indexBuffer->Map(0, &readRange, &indexDataBuffer); // �Ѳ��� ���ٰ� ǥ��.
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr); // �Ѳ��� �ݴ´�.

	// Initialize the vertex buffer view.
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // uint32�� ����ϰ� �־
	indexBufferView.SizeInBytes = bufferSize; // ������ ũ��

	IndexBufferInfo info =
	{
		indexBuffer,
		indexBufferView,
		DXGI_FORMAT_R32_UINT,
		indexCount
	};

	_vecIndexInfo.emplace_back(info);
}
