#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"

Mesh::Mesh()
	: Object(OBJECT_TYPE::MESH)
{
}

Mesh::~Mesh()
{
}

void Mesh::Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}

// Redner Begin�� End ���̿��� ����
void Mesh::Render(uint32 instanceCount)
{
	//GRAPHICS_CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �������� ���� ����
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15) vertex
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);	//�ε��� ���� �� ���

	// 1) Buffer���ٰ� ������ ����
	// 2) Table Descriptor Heap���ٰ� CBV ����
	// 3) ��� ������ ������, Table Decriptor Heap�� Ŀ��

	// GPU register�� �ִ� Table�� ����
	GEngine->GetGraphicsDescHeap()->CommitTable();

	// Input Assembler(IA) �ܰ迡 Vertex, Index������ �����ϴ� �ܰ�
	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);	// �����ϴ� �Ϳ� ������, RenderEnd���� ����, Vertex�� �̿�
	
	// Instancing ��� Ȱ�� ( Draw ���� ���� )
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);	//index �� �̿��� �׸��� ���. (�ξ� ȿ����!)
}

void Mesh::Render(shared_ptr<class InstancingBuffer>& buffer)
{
	D3D12_VERTEX_BUFFER_VIEW bufferView[] = { _vertexBufferView, buffer->GetBufferView() };
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferView);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, buffer->GetCount(), 0, 0, 0);
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
	_indexCount = static_cast<uint32>(buffer.size());	//render�� �� �ʿ��ϱ� ������, vertex ���� ����.
	uint32 bufferSize = _indexCount * sizeof(uint32);

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
		IID_PPV_ARGS(&_indexBuffer));
	// _indexBuffer : GPU ���� ������ ����Ŵ.

	void* indexDataBuffer = nullptr;	// �ﰢ�� �����͸� �ε��� ���ۿ� ����
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_indexBuffer->Map(0, &readRange, &indexDataBuffer); // �Ѳ��� ���ٰ� ǥ��.
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	_indexBuffer->Unmap(0, nullptr); // �Ѳ��� �ݴ´�.

	// Initialize the vertex buffer view.
	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT; // uint32�� ����ϰ� �־
	_indexBufferView.SizeInBytes = bufferSize; // ������ ũ��
}
