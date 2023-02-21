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

	ComPtr<ID3D12PipelineState> gfxPSO;
	ComPtr<ID3D12PipelineState> computePSO;

	Effect12(
		EffectType type,
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12PipelineLibrary> pipelineLibrary
	);
	
	GCRT_RESULT InitPipelineState(
		const EffectType type,
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12PipelineLibrary> pipelineLibrary
	);

	GCRT_RESULT InitPhongPSO(
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12PipelineLibrary> pipelineLibrary
	);

private:

	Effect12();
};