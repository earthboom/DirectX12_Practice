#pragma once

#include "Object.h"

enum class SHADER_TYPE : uint8
{
	DEFERRED,
	FORWARD,
	LIGHTING,
	COMPUTE,
};

enum class RASTERIZER_TYPE
{
	CULL_NONE,	// 무시하지 않음
	CULL_FRONT,	// 시계 방향 무시
	CULL_BACK,	// 반시계 방향 무시
	WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE
{
	LESS,
	LESS_EQUAL,		
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST,			//깊이 테스트 X + 깊이 기록 O
	NO_DEPTH_TEST_NO_WRITE,	//깊이 테스트 X + 깊이 기록 X
	LESS_NO_WRITE,			//깊이 테스트 O + 깊이 기록 X
};

// Pixel Shader의 결과물과 Render Target 결과물의 혼합 방법에 대한 옵션
enum class BLEND_TYPE : uint8
{
	DEFAULT,
	ALPHA_BLEND,
	ONE_TO_ONE_BLEND,
	END,
};

struct ShaderInfo
{
	// 기본 상태
	SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

// 일감 기술서에 비유
// 일을 처리함에 있어서 무엇을 해야할지를 기술함.

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void CreateGraphicsShader(const wstring& path, ShaderInfo info = ShaderInfo(), const string& vs = "VS_Main", const string& ps = "PS_Main");
	void CreateComputeShader(const wstring& path, const string& name, const string& version);
	void Update();

	SHADER_TYPE GetShaderType() { return _info.shaderType; }

private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:
	ShaderInfo _info;
	ComPtr<ID3D12PipelineState>			_pipelineState;

	// Graphics Shader 용도
	ComPtr<ID3DBlob>	_vsBlob;
	ComPtr<ID3DBlob>	_psBlob;
	ComPtr<ID3DBlob>	_errBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	_graphicsPipelineDesc = {};
	
	// Compute Shader 용도
	ComPtr<ID3DBlob>	_csBlob;
	D3D12_COMPUTE_PIPELINE_STATE_DESC	_computePipelineDesc = {};
};

