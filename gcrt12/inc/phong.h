#pragma once

#include "gcrtcommon12.h"

using Microsoft::WRL::ComPtr;
using namespace std;

GCRT_RESULT CreateResources(ComPtr<ID3D12Device> device);
GCRT_RESULT CreateCommandListAndFence(ComPtr<ID3D12Device> device);
GCRT_RESULT CreateDescriptorHeapsAndViews(ComPtr<ID3D12Device> device);