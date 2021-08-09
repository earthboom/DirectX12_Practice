#pragma once

// 계약서, 결재 단계 (비유)

// CPU가 GPU에게 일을 맡길 떄의 계약서, 결재의 단계를 의미.

// RootSignature의 최대 크기는 64 DWORD

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature> GetSignature() { return _signature; }

private:
	void CreateSamplerDesc();
	void CreateRootSignature();

public:
	ComPtr<ID3D12RootSignature> _signature;
	D3D12_STATIC_SAMPLER_DESC _samplerDesc;
};

