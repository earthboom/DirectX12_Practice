#include "pch.h"
#include "RootSignature.h"
#include "Engine.h"

void RootSignature::Init()
{
	//CD3DX12_ROOT_PARAMETER param[2];
	//param[0].InitAsConstantBufferView(0); // 0번 -> b0 -> CBV
	//param[1].InitAsConstantBufferView(1); // 1번 -> b1 -> CBV

	CreateSamplerDesc();
	CreateRootSignature();
}

void RootSignature::CreateSamplerDesc()
{
	_samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);	// 기본 sampler
}

void RootSignature::CreateRootSignature()
{
	//Table 내용물 설정
	// 이후 다른 내용 추가를 고려하여 배열로 만듦.
	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT - 1, 1),	//b1 ~ b4 (b0는 전역으로)
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0),	//t0 ~ t4
	};

	CD3DX12_ROOT_PARAMETER param[2];
	param[0].InitAsConstantBufferView(static_cast<uint32>(CBV_REGISTER::b0));	//B0을 전역으로 쓰기 위한 선언
	param[1].InitAsDescriptorTable(_countof(ranges), ranges);


	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 1, &_samplerDesc);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	//입력 조립기 단계

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}
