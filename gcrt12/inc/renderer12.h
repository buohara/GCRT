#pragma once

#include "gcrtcommon12.h"
#include "renderpass.h"

using Microsoft::WRL::ComPtr;
using namespace std;

enum AdapterSelectMethod
{
	PREFER_DISCRETE
};

struct GCRTAdapter
{
	string description;
	uint64_t vidMemBytes;
	uint64_t sysMemBytes;
	uint64_t sharedMemBytes;
	bool bHardware;
	bool bIntegrated;
	uint32_t dxCoreIdx;
};

struct Renderer12
{
	ComPtr<ID3D12Device> pDevice;
	vector<GCRTAdapter> adapters;

	ComPtr<ID3D12CommandQueue> gfxCQ;
	ComPtr<ID3D12CommandQueue> computeCQ;
	ComPtr<ID3D12CommandQueue> copyCQ;

	ComPtr<ID3D12PipelineLibrary> pipelineLibrary;

	string psoCachePath;

	Renderer12();
	~Renderer12();

	GCRT_RESULT GetSystemAdapters();
	GCRT_RESULT SelectAdapter(AdapterSelectMethod method);
	GCRT_RESULT InitializeDevice();
	GCRT_RESULT InitializeCQs();
	GCRT_RESULT InitializeEffects();
};