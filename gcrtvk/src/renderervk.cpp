#include "renderervk.h"

RenderSettingsVK g_settings = { 0 };

/**
 * WndProc Windows message handler.
 * @param  hWnd    Application window handle.
 * @param  message Message to process.
 * @param  wParam  Message parameters.
 * @param  lParam  More message parameters.
 * @return         Status code for message handler.
 */

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:

        break;

    case WM_CLOSE:

        exit(0);
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/**
 * [RendererVK::CreateVkInstance description]
 */

void RendererVK::CreateVkInstance()
{
    VkApplicationInfo appInfo   = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = "GCRTVK";
    appInfo.pEngineName         = "GCRTVK";
    appInfo.apiVersion          = VK_API_VERSION_1_0;

    vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
    instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#ifdef _DEBUG
    instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext                = NULL;
    instanceCreateInfo.pApplicationInfo     = &appInfo;
    
    instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

    const char *validationLayerNames[] = 
    {
            "VK_LAYER_GOOGLE_threading",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_swapchain",
            "VK_LAYER_GOOGLE_unique_objects"
    };

    instanceCreateInfo.enabledLayerCount = 6;
    instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;

    vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
}

/**
 * [RendererVK::CreateRenderWindow description]
 * @param hInstance [description]
 */

void RendererVK::CreateRenderWindow(HINSTANCE hInstance)
{
    WNDCLASS wc         = { 0 };
    wc.lpfnWndProc      = WndProc;
    wc.hInstance        = hInstance;
    wc.hbrBackground    = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName    = "GLWindow";
    wc.style = CS_OWNDC;

    if (!RegisterClass(&wc))
    {
        return;
    }

    // Create the main GL Window.

    hWnd = CreateWindowW(
        L"VKWindow",
        L"VKWindow",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0,
        0,
        g_settings.winW,
        g_settings.winH,
        0,
        0,
        hInstance,
        0
    );

    DWORD err = GetLastError();
    return;
}

/**
 * [RendererVK::CreateSwapChain description]
 * @param hInstance [description]
 */

void RendererVK::CreateSwapChain(HINSTANCE hInstance)
{
    VkResult err = VK_SUCCESS;

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType         = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance     = hInstance;
    surfaceCreateInfo.hwnd          = hWnd;

    err = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);

    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

    vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

    vector<VkBool32> supportsPresent(queueCount);
    for (uint32_t i = 0; i < queueCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]);
    }

    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    
    for (uint32_t i = 0; i < queueCount; i++)
    {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (graphicsQueueNodeIndex == UINT32_MAX)
            {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE)
            {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    
    if (presentQueueNodeIndex == UINT32_MAX)
    {
        for (uint32_t i = 0; i < queueCount; ++i)
        {
            if (supportsPresent[i] == VK_TRUE)
            {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }


    queueNodeIndex = graphicsQueueNodeIndex;

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
    vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

    if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        colorSpace = surfaceFormats[0].colorSpace;
    }
    else
    {
        bool found_B8G8R8A8_UNORM = false;
        for (auto&& surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                colorFormat = surfaceFormat.format;
                colorSpace = surfaceFormat.colorSpace;
                found_B8G8R8A8_UNORM = true;
                break;
            }
        }

        if (!found_B8G8R8A8_UNORM)
        {
            colorFormat = surfaceFormats[0].format;
            colorSpace = surfaceFormats[0].colorSpace;
        }
    }
}