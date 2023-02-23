#pragma once

#include "gcrtcommon12.h"

using Microsoft::WRL::ComPtr;
using namespace std;

enum EffectType
{
	PHONG,
};

struct Effect12
{
	EffectType type;

	ComPtr<ID3D12PipelineState> pGfxPSO;
	ComPtr<ID3D12PipelineState> pComputePSO;
	ComPtr<ID3D12CommandAllocator> pAllocator;
	ComPtr<ID3D12GraphicsCommandList> pCL;
	ComPtr<ID3D12Fence> pFence;

	uint32_t fenceVal;
	HANDLE fenceEvent;

	Effect12(
		EffectType type,
		ComPtr<ID3D12Device> pDevice,
		ComPtr<ID3D12PipelineLibrary> pPipelineLibrary
	);
	
	GCRT_RESULT InitPipelineState(
		const EffectType type,
		ComPtr<ID3D12Device> pDevice,
		ComPtr<ID3D12PipelineLibrary> pPipelineLibrary
	);

	GCRT_RESULT InitPhongPSO(
		ComPtr<ID3D12Device> pDevice,
		ComPtr<ID3D12PipelineLibrary> pPipelineLibrary
	);

private:

	Effect12();
};