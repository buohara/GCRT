#pragma once

#include "gcrtcommon12.h"

using Microsoft::WRL::ComPtr;
using namespace std;

struct GCRTAdapter
{
	string description;
	uint64_t vidMemBytes;
	uint64_t sysMemBytes;
	uint64_t sharedMemBytes;
	bool bHardware;
	bool bIntegrated;
};

struct Renderer12
{
	ComPtr<ID3D12Device> pDevice;
	vector<GCRTAdapter> adapters;

	Renderer12();

	GCRT_RESULT GetSystemAdapters();
	GCRT_RESULT SelectAdapter();
};