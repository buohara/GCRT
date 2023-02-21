#include "renderpass.h"
#include "phongPS_bc.h"
#include "phongVS_bc.h"

static D3D12_INPUT_ELEMENT_DESC elements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static const D3D12_INPUT_LAYOUT_DESC layout = { elements, _countof(elements) };

/**
 * @brief Initialize phong effect pipeline state and shaders.
 * 
 * @return GCRT_OK
 */

GCRT_RESULT Effect12::InitPhongPSO(
    ComPtr<ID3D12Device> device,
    ComPtr<ID3D12PipelineLibrary> pipelineLibrary
)
{
    ComPtr<ID3D12RootSignature> rootSig;

    assert(device->CreateRootSignature(0, phongPS_bc, sizeof(phongPS_bc), IID_PPV_ARGS(&rootSig)) == S_OK);

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

    HRESULT hr = pipelineLibrary->LoadGraphicsPipeline(L"Phong", &baseDesc, IID_PPV_ARGS(&gfxPSO));
    
    if (hr == E_INVALIDARG)
    {
        assert(device->CreateGraphicsPipelineState(&baseDesc, IID_PPV_ARGS(&gfxPSO)) == S_OK);
        assert(pipelineLibrary->StorePipeline(L"Phong", gfxPSO.Get()) == S_OK);
    }

	return GCRT_OK;
}