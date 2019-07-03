#include "renderervk.h"

RenderSettingsVK g_settings = { 0 };

PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
PFN_vkDebugReportMessageEXT dbgBreakCallback = VK_NULL_HANDLE;

VkDebugReportCallbackEXT msgCallback;

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject,
    size_t location,
    int32_t msgCode,
    const char* pLayerPrefix,
    const char* pMsg,
    void* pUserData)
{
    std::string prefix("");

    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        prefix += "ERROR:";
    };

    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        prefix += "WARNING:";
    };

    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        prefix += "PERFORMANCE:";
    };

    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        prefix += "INFO:";
    }

    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        prefix += "DEBUG:";
    }


    return VK_FALSE;
}

void setupDebugging(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack)
{
    CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));

    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
    dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
    dbgCreateInfo.flags = flags;

    VkResult err = CreateDebugReportCallback(
        instance,
        &dbgCreateInfo,
        nullptr,
        (callBack != VK_NULL_HANDLE) ? &callBack : &msgCallback);
    assert(!err);
}

void freeDebugCallback(VkInstance instance)
{
    if (msgCallback != VK_NULL_HANDLE)
    {
        DestroyDebugReportCallback(instance, msgCallback, nullptr);
    }
}

/**
 * RendererVK::RendererVK
 */

RendererVK::RendererVK(HINSTANCE hInstance)
{
    CreateVkInstance();
    CreateVkDevices();
    CreateRenderWindow(hInstance);
    GetPresentSurface(hInstance);
    CreateSwapChain();
    CreateDepth();
    SetupFrameBuffer();
}

/**
 * WndProc - Windows message handler.
 *
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
 * [RendererVK::CreateLogicalDevice description]
 *
 * @param enabledFeatures     [description]
 * @param enabledExtensions   [description]
 * @param useSwapChain        [description]
 * @param requestedQueueTypes [description]
 */

void RendererVK::CreateLogicalDevice(
    VkPhysicalDeviceFeatures enabledFeatures,
    vector<const char*> enabledExtensions,
    bool useSwapChain,
    VkQueueFlags requestedQueueTypes)
{
    vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    const float defaultQueuePriority(0.0f);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
    {
        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueIdx = i;
            }
        }

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex  = graphicsQueueIdx;
        queueInfo.queueCount        = 1;
        queueInfo.pQueuePriorities  = &defaultQueuePriority;
        
        queueCreateInfos.push_back(queueInfo);
    }

    if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                computeQueueIdx = i;
            }
        }

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex  = computeQueueIdx;
        queueInfo.queueCount        = 1;
        queueInfo.pQueuePriorities  = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    }

    if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
    {
        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                copyQueueIdx = i;
            }
        }

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex  = copyQueueIdx;
        queueInfo.queueCount        = 1;
        queueInfo.pQueuePriorities  = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    }

    vector<const char*> deviceExtensions(enabledExtensions);
    
    if (useSwapChain)
    {
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    VkDeviceCreateInfo deviceCreateInfo     = {};
    deviceCreateInfo.sType                  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount   = static_cast<uint32_t>(queueCreateInfos.size());;
    deviceCreateInfo.pQueueCreateInfos      = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures       = &enabledFeatures;
    
    if (deviceExtensions.size() > 0)
    {
        deviceCreateInfo.enabledExtensionCount      = (uint32_t)deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames    = deviceExtensions.data();
    }

    vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
}

/**
 * [RendererVK::Init description]
 */

void RendererVK::CreateVkDevices()
{
#ifdef _DEBUG
        VkDebugReportFlagsEXT debugReportFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        //setupDebugging(instance, debugReportFlags, VK_NULL_HANDLE);
#endif

    uint32_t gpuCount = 0;

    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    vector<VkPhysicalDevice> physicalDevices(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());

#ifdef _DEBUG
    
    printf("Available Vulkan devices\n");

    for (uint32_t i = 0; i < gpuCount; i++) 
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
        cout << "Device [" << i << "] : " << deviceProperties.deviceName << endl;
        cout << " Type: " << deviceProperties.deviceType << endl;
        cout << " API: " << (deviceProperties.apiVersion >> 22) << "." << ((deviceProperties.apiVersion >> 12) & 0x3ff) << "." << (deviceProperties.apiVersion & 0xfff) << endl;
    }

#endif
    uint32_t selectedDevice = 0;

    physicalDevice = physicalDevices[selectedDevice];

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
    
    VkPhysicalDeviceFeatures enabledFeatures = {};
    vector<const char*> enabledDeviceExtensions(0);

    CreateLogicalDevice(enabledFeatures, enabledDeviceExtensions);
    vkGetDeviceQueue(logicalDevice, graphicsQueueIdx, 0, &queue);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Get supported depth attachment formats.

    vector<VkFormat> depthFormats = 
    {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };

    for (auto& format : depthFormats)
    {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depthFormat = format;
        }
    }

    vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &presentComplete);
    vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &submitComplete);

    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask    = &flags;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &presentComplete;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &submitComplete;
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
        "VK_LAYER_LUNARG_standard_validation"
    };

    instanceCreateInfo.enabledLayerCount    = 1;
    instanceCreateInfo.ppEnabledLayerNames  = validationLayerNames;

    CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    setupDebugging(instance, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_NULL_HANDLE);
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
    wc.lpszClassName    = "VKWindow";
    wc.style            = CS_OWNDC;

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

void RendererVK::GetPresentSurface(HINSTANCE hInstance)
{
    VkResult err = VK_SUCCESS;

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType         = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance     = hInstance;
    surfaceCreateInfo.hwnd          = hWnd;

    CHECK_RESULT(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));

    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

    vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

    vector<VkBool32> supportsPresent(queueCount);
    for (uint32_t i = 0; i < queueCount; i++)
    {
        CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]));
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
    CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL));
    vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data()));

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

/**
 * [RendererVK::CreateSwapChain description]
 */

void RendererVK::CreateSwapChain()
{
    VkSwapchainKHR oldSwapchain = swapChain;
    VkSurfaceCapabilitiesKHR surfCaps;
    CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps));

    uint32_t presentModeCount;
    CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL));

    vector<VkPresentModeKHR> presentModes(presentModeCount);
    CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

    VkExtent2D swapchainExtent = {};

    if (surfCaps.currentExtent.width == 0xFFFFFFFF)
    {
        swapchainExtent.width   = g_settings.winW;
        swapchainExtent.height  = g_settings.winH;
    }
    else
    {
        swapchainExtent = surfCaps.currentExtent;
        g_settings.winW = surfCaps.currentExtent.width;
        g_settings.winH = surfCaps.currentExtent.height;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (size_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
    {
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfCaps.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = 
    {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (auto& compositeAlphaFlag : compositeAlphaFlags) 
    {
        if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) 
        {
            compositeAlpha = compositeAlphaFlag;
            break;
        };
    }

    VkSwapchainCreateInfoKHR swapchainCI    = {};
    swapchainCI.sType                       = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.pNext                       = NULL;
    swapchainCI.surface                     = surface;
    swapchainCI.minImageCount               = desiredNumberOfSwapchainImages;
    swapchainCI.imageFormat                 = colorFormat;
    swapchainCI.imageColorSpace             = colorSpace;
    swapchainCI.imageExtent                 = { swapchainExtent.width, swapchainExtent.height };
    swapchainCI.imageUsage                  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.preTransform                = (VkSurfaceTransformFlagBitsKHR)preTransform;
    swapchainCI.imageArrayLayers            = 1;
    swapchainCI.imageSharingMode            = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCI.queueFamilyIndexCount       = 0;
    swapchainCI.pQueueFamilyIndices         = NULL;
    swapchainCI.presentMode                 = swapchainPresentMode;
    swapchainCI.oldSwapchain                = oldSwapchain;

    swapchainCI.clipped                     = VK_TRUE;
    swapchainCI.compositeAlpha              = compositeAlpha;

    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) 
    {
        swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) 
    {
        swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    CHECK_RESULT(vkCreateSwapchainKHR(logicalDevice, &swapchainCI, nullptr, &swapChain));

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        for (uint32_t i = 0; i < scSize; i++)
        {
            vkDestroyImageView(logicalDevice, scImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
    }

    CHECK_RESULT(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &scSize, NULL));

    scImages.resize(scSize);
    CHECK_RESULT(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &scSize, scImages.data()));

    scImageViews.resize(scSize);
    
    for (uint32_t i = 0; i < scSize; i++)
    {
        VkImageViewCreateInfo colorAttachmentView   = {};
        colorAttachmentView.sType                   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorAttachmentView.pNext                   = NULL;
        colorAttachmentView.format                  = colorFormat;
        
        colorAttachmentView.components = 
        {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A
        };
        
        colorAttachmentView.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        colorAttachmentView.subresourceRange.baseMipLevel   = 0;
        colorAttachmentView.subresourceRange.levelCount     = 1;
        colorAttachmentView.subresourceRange.baseArrayLayer = 0;
        colorAttachmentView.subresourceRange.layerCount     = 1;
        colorAttachmentView.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        colorAttachmentView.flags                           = 0;

        colorAttachmentView.image = scImages[i];

        CHECK_RESULT(vkCreateImageView(logicalDevice, &colorAttachmentView, nullptr, &scImageViews[i]));
    }
}

/**
 * RendererVK::Render Acquire next swapchain buffer, execute command buffers, and present.
 */

void RendererVK::Render(SceneVk& scn)
{
    vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, presentComplete, (VkFence)nullptr, &curSCBuf);
 
    for (auto& pass : passes)
    {
        vkWaitForFences(logicalDevice, 1, &pass.fences[curSCBuf], VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &pass.fences[curSCBuf]);

        pass.BuildCommandBuffers(logicalDevice, curSCBuf, scn);

        VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {};

        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pWaitDstStageMask    = &waitStageMask;
        submitInfo.pWaitSemaphores      = &presentComplete;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pSignalSemaphores    = &submitComplete;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pCommandBuffers      = &pass.cmdBuffers[curSCBuf];
        submitInfo.commandBufferCount   = 1;

        vkQueueSubmit(queue, 1, &submitInfo, pass.fences[curSCBuf]);
    }

    VkPresentInfoKHR presentInfo    = {};
    presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext               = NULL;
    presentInfo.swapchainCount      = 1;
    presentInfo.pSwapchains         = &swapChain;
    presentInfo.pImageIndices       = &curSCBuf;

    if (submitComplete != VK_NULL_HANDLE)
    {
        presentInfo.pWaitSemaphores     = &submitComplete;
        presentInfo.waitSemaphoreCount  = 1;
    }
 
    vkQueuePresentKHR(queue, &presentInfo);
}

/**
 * RendererVK::SetupFrameBuffer Find final render pass from list of passes and attach
 * default framebuffer to it.
 */

void RendererVK::SetupFrameBuffer()
{
    VkImageView attachments[2];
    attachments[1] = zView;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    
    frameBufferCreateInfo.sType             = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext             = NULL;
    frameBufferCreateInfo.attachmentCount   = 2;
    frameBufferCreateInfo.pAttachments      = attachments;
    frameBufferCreateInfo.width             = g_settings.winW;
    frameBufferCreateInfo.height            = g_settings.winH;
    frameBufferCreateInfo.layers            = 1;

    frameBuffers.resize(scSize);
    for (uint32_t i = 0; i < frameBuffers.size(); i++)
    {
        attachments[0] = scImageViews[i];

        for (auto& pass : passes)
        {
            if (pass.renderToFrameBuffer == true)
            {
                frameBufferCreateInfo.renderPass = pass.renderPass;
                vkCreateFramebuffer(logicalDevice, &frameBufferCreateInfo, nullptr, &pass.frameBuffers[i]);
                break;
            }
        }  
    }
}

/**
 * RendererVK::CreateDepth Default frame buffer also has depth attachment. Create it here.
 */

void RendererVK::CreateDepth()
{
    VkImageCreateInfo image = {};

    image.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.pNext         = NULL;
    image.imageType     = VK_IMAGE_TYPE_2D;
    image.format        = depthFormat;
    image.extent        = { g_settings.winW, g_settings.winH, 1 };
    image.mipLevels     = 1;
    image.arrayLayers   = 1;
    image.samples       = VK_SAMPLE_COUNT_1_BIT;
    image.tiling        = VK_IMAGE_TILING_OPTIMAL;
    image.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image.flags         = 0;

    VkMemoryAllocateInfo mem_alloc = {};

    mem_alloc.sType             = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext             = NULL;
    mem_alloc.allocationSize    = 0;
    mem_alloc.memoryTypeIndex   = 0;

    VkImageViewCreateInfo depthStencilView = {};

    depthStencilView.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthStencilView.pNext                              = NULL;
    depthStencilView.viewType                           = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format                             = depthFormat;
    depthStencilView.flags                              = 0;
    depthStencilView.subresourceRange                   = {};
    depthStencilView.subresourceRange.aspectMask        = VK_IMAGE_ASPECT_DEPTH_BIT;
    depthStencilView.subresourceRange.baseMipLevel      = 0;
    depthStencilView.subresourceRange.levelCount        = 1;
    depthStencilView.subresourceRange.baseArrayLayer    = 0;
    depthStencilView.subresourceRange.layerCount        = 1;

    VkMemoryRequirements memReqs;

    vkCreateImage(logicalDevice, &image, nullptr, &zImage);
    vkGetImageMemoryRequirements(logicalDevice, zImage, &memReqs);
    mem_alloc.allocationSize = memReqs.size;
    
    mem_alloc.memoryTypeIndex = FindProperties(
        memReqs.memoryTypeBits, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        deviceMemoryProperties
    );

    vkAllocateMemory(logicalDevice, &mem_alloc, nullptr, &zMem);
    vkBindImageMemory(logicalDevice, zImage, zMem, 0);

    depthStencilView.image = zImage;
    vkCreateImageView(logicalDevice, &depthStencilView, nullptr, &zView);
}

void RendererVK::Upload(SceneVk& scn)
{
    VkSubmitInfo submitInfo = {};

    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers      = &scn.uploadBuf;
    submitInfo.commandBufferCount   = 1;

    vkQueueSubmit(queue, 1, &submitInfo, scn.uploadFence);
    vkWaitForFences(logicalDevice, 1, &scn.uploadFence, VK_TRUE, UINT64_MAX);
}