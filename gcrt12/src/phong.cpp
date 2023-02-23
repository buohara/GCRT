#include "renderpass.h"
#include "phong.h"
#include "phongPS_bc.h"
#include "phongVS_bc.h"

static D3D12_INPUT_ELEMENT_DESC elements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static const D3D12_INPUT_LAYOUT_DESC layout = { elements, _countof(elements) };

/**
 * @brief Initialize phong effect pipeline state and shaders.
 * 
 * @return GCRT_OK
 */

GCRT_RESULT Effect12::InitPhongPSO(
    ComPtr<ID3D12Device> pDevice,
    ComPtr<ID3D12PipelineLibrary> pPipelineLibrary
)
{
    ComPtr<ID3D12RootSignature> rootSig;

    assert(pDevice->CreateRootSignature(0, phongPS_bc, sizeof(phongPS_bc), IID_PPV_ARGS(&rootSig)) == S_OK);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc = {};
    baseDesc.pRootSignature                     = rootSig.Get();
    baseDesc.SampleMask                         = UINT_MAX;
    baseDesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    baseDesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    baseDesc.NumRenderTargets                   = 1;
    baseDesc.RTVFormats[0]                      = DXGI_FORMAT_R8G8B8A8_UNORM;
    baseDesc.SampleDesc.Count                   = 1;
    baseDesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    baseDesc.DepthStencilState.DepthEnable      = FALSE;
    baseDesc.DepthStencilState.StencilEnable    = FALSE;

    baseDesc.InputLayout                        = layout;
    baseDesc.VS.pShaderBytecode                 = phongVS_bc;
    baseDesc.VS.BytecodeLength                  = sizeof(phongVS_bc);
    baseDesc.PS.pShaderBytecode                 = phongPS_bc;
    baseDesc.PS.BytecodeLength                  = sizeof(phongPS_bc);

    HRESULT hr = pPipelineLibrary->LoadGraphicsPipeline(L"Phong", &baseDesc, IID_PPV_ARGS(&pGfxPSO));
    
    if (hr == E_INVALIDARG)
    {
        assert(pDevice->CreateGraphicsPipelineState(&baseDesc, IID_PPV_ARGS(&pGfxPSO)) == S_OK);
        assert(pPipelineLibrary->StorePipeline(L"Phong", pGfxPSO.Get()) == S_OK);
    }

	return GCRT_OK;
}

/**
 * @brief Initialize phong effect pipeline state and shaders.
 *
 * @param device D3D device to create resources with.
 * 
 * @return GCRT_OK
 */

GCRT_RESULT CreateResources(ComPtr<ID3D12Device> pDevice)
{
    /*D3D12_DESCRIPTOR_HEAP_DESC srvUavHeapDesc   = {};
    srvUavHeapDesc.NumDescriptors               = 2;
    srvUavHeapDesc.Type                         = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvUavHeapDesc.Flags                        = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    assert(pDevice->CreateDescriptorHeap(&srvUavHeapDesc, IID_PPV_ARGS(&srvUavHeap.pHeap)) == S_OK);

    srvUavDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.SizeInBytes = cbSize;
    cbvDesc.BufferLocation = constants.pResource->GetGPUVirtualAddress();

    D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle;
    cbvHandle.ptr = srvUavHeap.pHeap->GetCPUDescriptorHandleForHeapStart().ptr;
    device.pDevice->CreateConstantBufferView(&cbvDesc, cbvHandle);

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    D3D12_CPU_DESCRIPTOR_HANDLE uavHandle;
    uavHandle.ptr = srvUavHeap.pHeap->GetCPUDescriptorHandleForHeapStart().ptr + 2 * srvUavDescriptorSize;
    device.pDevice->CreateUnorderedAccessView(dst.pResource.Get(), nullptr, &uavDesc, uavHandle);*/

    return GCRT_OK;
}

/**
 * @brief Initialize phong effect pipeline state and shaders.
 *
 * @param device D3D device to create resources with.
 *
 * @return GCRT_OK
 */

GCRT_RESULT CreateCommandListAndFence(
    ComPtr<ID3D12Device> pDevice,
    ComPtr<ID3D12GraphicsCommandList> pCL,
    ComPtr<ID3D12Fence> pFence,
    ComPtr<ID3D12CommandAllocator> pAllocator,
    uint32_t &fenceVal,
    HANDLE &fenceEvent
)
{
    pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&pAllocator));
    pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, pAllocator.Get(), nullptr, IID_PPV_ARGS(&pCL));
    pCL->Close();

    pDevice->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    fenceVal++;

    return GCRT_OK;
}

GCRT_RESULT CreateDescriptorHeapsAndViews(ComPtr<ID3D12Device> device)
{
    return GCRT_OK;
}