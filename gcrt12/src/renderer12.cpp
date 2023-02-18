#include "renderer12.h"

using namespace std;

/**
 * @brief Renderer constructor. Select a D3D adapter, create a device, set up
 * command queues, and initialize render modules.
 */

Renderer12::Renderer12()
{
    GetSystemAdapters();
    SelectAdapter(PREFER_DISCRETE);
    InitializeDevice();
}

/**
 * @brief Compare adapters with preference for discrete with large
 * VRAM.
 * 
 * @param a1 [in] First adapter to compare.
 * @param a2 [in] Second adapter to compare.
 * 
 * @return True if first adapter is HW and second isn't, or first has more VRAM. 
 */

bool CompareDiscrete(GCRTAdapter& a1, GCRTAdapter& a2)
{
    if (a1.bHardware && !a2.bHardware)
        return true;

    return a1.vidMemBytes > a2.vidMemBytes;
}

/**
 * @brief Select from a list of D3D adapters in the system based on
 * input heuristic.
 * 
 * @param method Input heuristic for adapter selction.
 * @return GCRT_OK
 */

GCRT_RESULT Renderer12::SelectAdapter(AdapterSelectMethod method)
{
    switch (method)
    {
        case PREFER_DISCRETE:
        default:

            sort(adapters.begin(), adapters.end(), CompareDiscrete);
            break;
    }

    return GCRT_OK;
}

/**
 * @brief Populate a list of D3D adapters in the system and their properties.
 * 
 * @return GCRT_OK
 */

GCRT_RESULT Renderer12::GetSystemAdapters()
{
    ComPtr<IDXCoreAdapterFactory> pCoreFactory;
    ComPtr<IDXCoreAdapterList> pAdapterList;

    GUID attributes[] = { DXCORE_ADAPTER_ATTRIBUTE_D3D12_GRAPHICS };

    assert(DXCoreCreateAdapterFactory(IID_PPV_ARGS(&pCoreFactory)) == S_OK);
    assert(pCoreFactory->CreateAdapterList(_countof(attributes), attributes, IID_PPV_ARGS(&pAdapterList)) == S_OK);

    const uint32_t count = pAdapterList->GetAdapterCount();

    for (uint32_t i = 0; i < count; i++)
    {
        GCRTAdapter newAdapter;

        newAdapter.dxCoreIdx = i;

        ComPtr<IDXCoreAdapter> candidateAdapter;
        pAdapterList->GetAdapter(i, IID_PPV_ARGS(&candidateAdapter));

        bool isHardware = false;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::IsHardware, &isHardware);
        newAdapter.bHardware = isHardware;

        bool isIntegrated = false;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::IsIntegrated, &isIntegrated);
        newAdapter.bIntegrated = isIntegrated;

        uint64_t vidmem = 0;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::DedicatedAdapterMemory, &vidmem);
        newAdapter.vidMemBytes = vidmem;

        uint64_t sysmem = 0;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::DedicatedSystemMemory, &sysmem);
        newAdapter.sysMemBytes = sysmem;

        uint64_t sharedMem = 0;
        candidateAdapter->GetProperty(DXCoreAdapterProperty::SharedSystemMemory, &sharedMem);
        newAdapter.sharedMemBytes = sharedMem;

        char driverDesc[256];
        candidateAdapter->GetProperty(DXCoreAdapterProperty::DriverDescription, &driverDesc);
        newAdapter.description = string(driverDesc);

        adapters.push_back(newAdapter);
    }

    return GCRT_OK;
}

/**
 * @brief After selecting from a list of adapter, create a D3D12 device on it.
 * 
 * @return GCRT_OK
 */

GCRT_RESULT Renderer12::InitializeDevice()
{
    ComPtr<IDXCoreAdapterFactory> pCoreFactory;
    ComPtr<IDXCoreAdapterList> pAdapterList;
    ComPtr<IDXCoreAdapter> pAdapter;

    GUID attributes[] = { DXCORE_ADAPTER_ATTRIBUTE_D3D12_GRAPHICS };

    assert(DXCoreCreateAdapterFactory(IID_PPV_ARGS(&pCoreFactory)) == S_OK);
    assert(pCoreFactory->CreateAdapterList(_countof(attributes), attributes, IID_PPV_ARGS(&pAdapterList)) == S_OK);
    assert(pAdapterList->GetAdapter(adapters[0].dxCoreIdx, IID_PPV_ARGS(&pAdapter)) == S_OK);

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

    assert(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pDevice)) == S_OK);

    return GCRT_OK;
}

/**
 * @brief Initialize renderer command queues.
 *
 * @return GCRT_OK
 */

GCRT_RESULT Renderer12::InitializeCQs()
{
    D3D12_COMMAND_QUEUE_DESC directQueueDesc = {};
    directQueueDesc.Flags   = D3D12_COMMAND_QUEUE_FLAG_NONE;
    directQueueDesc.Type    = D3D12_COMMAND_LIST_TYPE_DIRECT;

    assert(pDevice->CreateCommandQueue(&directQueueDesc, IID_PPV_ARGS(&gfxCQ)) == S_OK);

    D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
    copyQueueDesc.Flags     = D3D12_COMMAND_QUEUE_FLAG_NONE;
    copyQueueDesc.Type      = D3D12_COMMAND_LIST_TYPE_COPY;

    assert(pDevice->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&gfxCQ)) == S_OK);

    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    computeQueueDesc.Flags  = D3D12_COMMAND_QUEUE_FLAG_NONE;
    computeQueueDesc.Type   = D3D12_COMMAND_LIST_TYPE_COMPUTE;

    assert(pDevice->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&gfxCQ)) == S_OK);

    return GCRT_OK;
}