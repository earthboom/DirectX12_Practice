#pragma once

#include "Object.h"

// 일감 기술서에 비유
// 일을 처리함에 있어서 무엇을 해야할지를 기술함.

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void Init(const wstring& path);
	void Update();

private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:
	ComPtr<ID3DBlob>	_vsBlob;
	ComPtr<ID3DBlob>	_psBlob;
	ComPtr<ID3DBlob>	_errBlob;

	ComPtr<ID3D12PipelineState>			_pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	_pipelineDesc = {};
};

