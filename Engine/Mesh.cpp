#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"
#include "FBXLoader.h"
#include "StructuredBuffer.h"

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

shared_ptr<Mesh> Mesh::CreateFromFBX(const FbxMeshInfo* meshInfo, FBXLoader& loader)
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

	// animation이 있다면
	if (meshInfo->hasAnimation)
		mesh->CreateBonesAndAnimations(loader);

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

// 실직적으로 animtion 과 Skin data를 얻음
void Mesh::CreateBonesAndAnimations(FBXLoader& loader)
{
#pragma region AnimClip
	uint32 frameCount = 0;
	vector<shared_ptr<FbxAnimClipInfo>>& animClips = loader.GetAnimClip();
	for (shared_ptr<FbxAnimClipInfo>& ac : animClips)
	{
		AnimClipInfo info = {};

		info.animName = ac->name;
		info.duration = ac->endTime.GetSecondDouble() - ac->startTime.GetSecondDouble();

		int32 startFrame = static_cast<int32>(ac->startTime.GetFrameCount(ac->mode));
		int32 endFrame = static_cast<int32>(ac->endTime.GetFrameCount(ac->mode));
		info.frameCount = endFrame - startFrame;

		info.keyFrames.resize(ac->keyFrames.size());

		const int32 boneCount = static_cast<int32>(ac->keyFrames.size());
		for (int32 b = 0; b < boneCount; b++)
		{
			auto& vec = ac->keyFrames[b];

			const int32 size = static_cast<int32>(vec.size());
			frameCount = max(frameCount, static_cast<uint32>(size));
			info.keyFrames[b].resize(size);

			for (int32 f = 0; f < size; f++)
			{
				FbxKeyFrameInfo& kf = vec[f];
				// FBX에서 파싱한 정보들로 채워준다
				KeyFrameInfo& kfInfo = info.keyFrames[b][f];
				kfInfo.time = kf.time;
				kfInfo.frame = static_cast<int32>(size);
				kfInfo.scale.x = static_cast<float>(kf.matTransform.GetS().mData[0]);
				kfInfo.scale.y = static_cast<float>(kf.matTransform.GetS().mData[1]);
				kfInfo.scale.z = static_cast<float>(kf.matTransform.GetS().mData[2]);
				kfInfo.rotation.x = static_cast<float>(kf.matTransform.GetQ().mData[0]);
				kfInfo.rotation.y = static_cast<float>(kf.matTransform.GetQ().mData[1]);
				kfInfo.rotation.z = static_cast<float>(kf.matTransform.GetQ().mData[2]);
				kfInfo.rotation.w = static_cast<float>(kf.matTransform.GetQ().mData[3]);
				kfInfo.translate.x = static_cast<float>(kf.matTransform.GetT().mData[0]);
				kfInfo.translate.y = static_cast<float>(kf.matTransform.GetT().mData[1]);
				kfInfo.translate.z = static_cast<float>(kf.matTransform.GetT().mData[2]);
			}
		}

		_animClips.emplace_back(info);
	}
#pragma endregion

#pragma region Bones
	vector<shared_ptr<FbxBoneInfo>>& bones = loader.GetBones();
	for (shared_ptr<FbxBoneInfo>& bone : bones)
	{
		BoneInfo boneInfo = {};
		boneInfo.parentIdx = bone->parentIndex;
		boneInfo.matOffset = GetMatrix(bone->matOffset);
		boneInfo.boneName = bone->boneName;
		_bones.emplace_back(boneInfo);
	}
#pragma endregion

#pragma region SkinData
	if (IsAnimMesh())
	{
		// BoneOffet 행렬
		const int32 boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++)
			offsetVec[b] = _bones[b].matOffset;

		// OffsetMatrix StructuredBuffer 세팅
		_offsetBuffer = make_shared<StructuredBuffer>();
		_offsetBuffer->Init(sizeof(Matrix), static_cast<uint32>(offsetVec.size()), offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo& animClip = _animClips[i];

			// 애니메이션 프레임 정보
			vector<AnimFrameParams> frameParams;
			frameParams.resize(_bones.size() * animClip.frameCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				const int32 keyFrameCount = static_cast<int32>(animClip.keyFrames[b].size());
				for (int32 f = 0; f < keyFrameCount; f++)
				{
					int32 idx = static_cast<int32>(boneCount * f + b);

					frameParams[idx] = AnimFrameParams
					{
						Vec4(animClip.keyFrames[b][f].scale),
						animClip.keyFrames[b][f].rotation, // Quaternion
						Vec4(animClip.keyFrames[b][f].translate)
					};
				}
			}

			// StructuredBuffer 세팅
			_frameBuffer.push_back(make_shared<StructuredBuffer>());
			_frameBuffer.back()->Init(sizeof(AnimFrameParams), static_cast<uint32>(frameParams.size()), frameParams.data());
		}
	}
#pragma endregion
}

Matrix Mesh::GetMatrix(FbxAMatrix& matrix)
{
	Matrix mat;

	for (int32 y = 0; y < 4; ++y)
		for (int32 x = 0; x < 4; ++x)
			mat.m[y][x] = static_cast<float>(matrix.Get(y, x));

	return mat;
}
