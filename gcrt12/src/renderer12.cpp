#include "renderer12.h"

Renderer12::Renderer12()
{
    GetSystemAdapters();
    SelectAdapter();
}

GCRT_RESULT Renderer12::SelectAdapter()
{
    return GCRT_OK;
}

GCRT_RESULT Renderer12::GetSystemAdapters()
{
    ComPtr<IDXGIFactory> pDXGIFactory;
    ComPtr<IDXCoreAdapterFactory> pCoreFactory;
    ComPtr<IDXCoreAdapterList> pAdapterList;

    uint32_t dxgiFactoryFlags = 0;

#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debugController;

        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    assert(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pDXGIFactory)) == S_OK);
    assert(DXCoreCreateAdapterFactory(IID_PPV_ARGS(&pCoreFactory)) == S_OK);

    GUID attributes[] = { DXCORE_ADAPTER_ATTRIBUTE_D3D12_GRAPHICS };
    pCoreFactory->CreateAdapterList(_countof(attributes), attributes, IID_PPV_ARGS(&pAdapterList));

    const uint32_t count = pAdapterList->GetAdapterCount();

    for (uint32_t i = 0; i < count; ++i)
    {
        GCRTAdapter newAdapter;

        ComPtr<IDXCoreAdapter> candidateAdapter;
        pAdapterList->GetAdapter(i, IID_PPV_ARGS(&candidateAdapter));

        bool isHardware = false;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::IsHardware, &isHardware);
        newAdapter.bHardware = isHardware;

        uint64_t vidmem = 0;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::DedicatedAdapterMemory, &vidmem);
        newAdapter.vidMemBytes = vidmem;

        uint64_t sysmem = 0;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::DedicatedSystemMemory, &sysmem);
        newAdapter.sysMemBytes = sysmem;

        uint64_t sharedMem = 0;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::SharedSystemMemory, &sharedMem);
        newAdapter.sharedMemBytes = sharedMem;

        adapters.push_back(newAdapter);
    }

    return GCRT_OK;
}