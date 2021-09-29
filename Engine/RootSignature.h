#pragma once

// 계약서, 결재 단계 (비유)

// CPU가 GPU에게 일을 맡길 떄의 계약서, 결재의 단계를 의미.

// RootSignature의 최대 크기는 64 DWORD

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return _computeRootSignature; }

private:
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

public:
	D3D12_STATIC_SAMPLER_DESC _samplerDesc;
	ComPtr<ID3D12RootSignature> _graphicsRootSignature;	
	ComPtr<ID3D12RootSignature> _computeRootSignature;
};


