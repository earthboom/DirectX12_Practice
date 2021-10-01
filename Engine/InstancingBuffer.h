#pragma once

// 버퍼에게 넘겨줄 각 물체의 행렬 좌표 데이터
struct InstancingParams
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
};

class InstancingBuffer
{
public:
	InstancingBuffer();
	~InstancingBuffer();

	void Init(uint32 maxCount = 10);

	void Clear();
	void AddData(InstancingParams& params);
	void PushData();

public:
	uint32						GetCount() { return static_cast<uint32>(_data.size()); }
	ComPtr<ID3D12Resource>		GetBuffer() { return _buffer; }
	D3D12_VERTEX_BUFFER_VIEW	GetBufferView() { return _bufferView; }
	
	void SetID(uint64 instanceId) { _instancingId = instanceId; }
	uint64 GetID() { return _instancingId; }

private:
	uint64						_instancingId = 0;
	ComPtr<ID3D12Resource>		_buffer;
	D3D12_VERTEX_BUFFER_VIEW	_bufferView;

	uint32						_maxCount = 0;
	vector<InstancingParams>	_data;	// Parameter 저장
};

