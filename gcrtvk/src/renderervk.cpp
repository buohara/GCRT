#include "renderervk.h"

RenderSettingsVK g_settings = { 0 };

VkDebugUtilsMessengerEXT debugMessenger;

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param messageSeverity [description]
 * @param messageType [description]
 * @param pCallbackData [description]
 * @param pUserData [description]
 * @return [description]
 */

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    printf("validation layer: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

/**
 * 
 */

VkResult setupDebugging(
    VkInstance instance,
    const VkAllocationCallbacks* pAllocator
)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType            = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity  = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType      = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback   = debugCallback;
    createInfo.pUserData        = nullptr;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr) return func(instance, &createInfo, pAllocator, &debugMessenger);
    else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param hInstance [description]
 */

RendererVK::RendererVK(HINSTANCE hInstance)
{
    CreateVkInstance();
    CreateVkDevices();
    CreateRenderWindow(hInstance);
    GetPresentSurface(hInstance);
    CreateSwapChain();
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param hWnd [description]
 * @param message [description]
 * @param wParam [description]
 * @param lParam [description]
 * @return [description]
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
 * @brief [brief description]
 * @details [long description]
 * 
 * @param enabledFeatures [description]
 * @param enabledExtensions [description]
 * @param useSwapChain [description]
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
 * @brief [brief description]
 * @details [long description]
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
 * @brief [brief description]
 * @details [long description]
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

    setupDebugging(instance, nullptr);
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
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

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param hInstance [description]
 */

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
 * @brief [brief description]
 * @details [long description]
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
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;

    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else preTransform = surfCaps.currentTransform;

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

    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    CHECK_RESULT(vkCreateSwapchainKHR(logicalDevice, &swapchainCI, nullptr, &swapChain));

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        for (uint32_t i = 0; i < scSize; i++) vkDestroyImageView(logicalDevice, scImageViews[i], nullptr);
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
 * @brief [brief description]
 * @details [long description]
 * 
 * @param scn [description]
 * @param n [description]
 * @param X [description]
 * @param e [description]
 * @param r [description]
 * @param f [description]
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
 * @brief [brief description]
 * @details [long description]
 * 
 * @param scn [description]
 */

void RendererVK::Upload(SceneVk& scn)
{
    VkSubmitInfo submitInfo = {};

    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers      = &scn.uploadBuf;
    submitInfo.commandBufferCount   = 1;

    vkQueueSubmit(queue, 1, &submitInfo, scn.uploadFence);
    vkWaitForFences(logicalDevice, 1, &scn.uploadFence, VK_TRUE, UINT64_MAX);
}