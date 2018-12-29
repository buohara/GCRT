#include "renderervk.h"

RenderSettingsVK g_settings = { 0 };

/**
 * [findProperties description]
 * @param  pMemoryProperties         [description]
 * @param  memoryTypeBitsRequirement [description]
 * @param  requiredProperties        [description]
 * @return                           [description]
 */

uint32_t RendererVK::FindProperties(
    uint32_t memoryTypeBitsRequirement,
    VkMemoryPropertyFlags requiredProperties
) 
{
    const uint32_t memoryCount = deviceMemoryProperties.memoryTypeCount;
    
    for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) 
    {
        const uint32_t memoryTypeBits = (1 << memoryIndex);
        const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;

        const VkMemoryPropertyFlags properties =
            deviceMemoryProperties.memoryTypes[memoryIndex].propertyFlags;
        
        const bool hasRequiredProperties =
            (properties & requiredProperties) == requiredProperties;

        if (isRequiredMemoryType && hasRequiredProperties)
        {
            return (uint32_t)memoryIndex;
        }
    }

    return ~0;
}

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
 * [RendererVK::CreateLogicalDevice description]
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
    CreateCommandPool(graphicsQueueIdx);
}

/**
 * [RendererVK::Init description]
 */

void RendererVK::Init()
{
    CreateVkInstance();

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

/**
 * [RendererVK::CreateSwapChain description]
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
 * RendererVK::CreateCommandPool
 */

void RendererVK::CreateCommandPool(uint32_t queueIdx)
{
    VkCommandPoolCreateInfo cmdPoolInfo     = {};
    cmdPoolInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex            = queueIdx;
    cmdPoolInfo.flags                       = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &cmdPool);
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

    if (surfCaps.currentExtent.width == ~0)
    {
        swapchainExtent.width   = g_settings.winW;
        swapchainExtent.height  = g_settings.winH;
    }
    else
    {
        swapchainExtent = surfCaps.currentExtent;
        g_settings.winW = surfCaps.currentExtent.width;
        g_settings.winW = surfCaps.currentExtent.height;
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
 * [RendererVK::CreateCommandBuffers description]
 */

void RendererVK::CreateCommandBuffers()
{
    drawCmdBuffers.resize(scSize);

    VkCommandBufferAllocateInfo cmdBufferAllocateInfo   = {};
    cmdBufferAllocateInfo.sType                         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocateInfo.commandPool                   = cmdPool;
    cmdBufferAllocateInfo.level                         = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocateInfo.commandBufferCount            = scSize;

    vkAllocateCommandBuffers(logicalDevice, &cmdBufferAllocateInfo, drawCmdBuffers.data());
}

/**
 * [RendererVK::CreateFenceObjects description]
 */

void RendererVK::CreateFenceObjects()
{
    VkFenceCreateInfo fenceCreateInfo   = {};
    fenceCreateInfo.sType               = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags               = 0;

    fences.resize(drawCmdBuffers.size());
    
    for (auto& fence : fences) 
    {
        vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence);
    }
}

/**
 * [RendererVK::CreateDepth description]
 */

void RendererVK::CreateDepth()
{
    VkImageCreateInfo image     = {};
    image.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.pNext                 = NULL;
    image.imageType             = VK_IMAGE_TYPE_2D;
    image.format                = depthFormat;
    image.extent                = { g_settings.winW, g_settings.winH, 1 };
    image.mipLevels             = 1;
    image.arrayLayers           = 1;
    image.samples               = VK_SAMPLE_COUNT_1_BIT;
    image.tiling                = VK_IMAGE_TILING_OPTIMAL;
    image.usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image.flags                 = 0;

    VkMemoryAllocateInfo mem_alloc  = {};
    mem_alloc.sType                 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext                 = NULL;
    mem_alloc.allocationSize        = 0;
    mem_alloc.memoryTypeIndex       = 0;

    VkImageViewCreateInfo depthStencilView              = {};
    depthStencilView.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthStencilView.pNext                              = NULL;
    depthStencilView.viewType                           = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format                             = depthFormat;
    depthStencilView.flags                              = 0;
    depthStencilView.subresourceRange                   = {};
    depthStencilView.subresourceRange.aspectMask        = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel      = 0;
    depthStencilView.subresourceRange.levelCount        = 1;
    depthStencilView.subresourceRange.baseArrayLayer    = 0;
    depthStencilView.subresourceRange.layerCount        = 1;

    VkMemoryRequirements memReqs;

    vkCreateImage(logicalDevice, &image, nullptr, &zImage);
    vkGetImageMemoryRequirements(logicalDevice, zImage, &memReqs);
    mem_alloc.allocationSize = memReqs.size;
    mem_alloc.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    vkAllocateMemory(logicalDevice, &mem_alloc, nullptr, &zMem);
    vkBindImageMemory(logicalDevice, zImage, zMem, 0);

    depthStencilView.image = zImage;
    vkCreateImageView(logicalDevice, &depthStencilView, nullptr, &zView);
}

/**
 * RendererVK::CreateRenderPass Setup global renderpass for writing to frame buffer.
 */

void RendererVK::CreateRenderPass()
{
    array<VkAttachmentDescription, 2> attachments = {};

    attachments[0].format           = colorFormat;
    attachments[0].samples          = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    attachments[1].format           = depthFormat;
    attachments[1].samples          = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout      = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference    = {};
    colorReference.attachment               = 0;
    colorReference.layout                   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference    = {};
    depthReference.attachment               = 1;
    depthReference.layout                   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription         = {};
    subpassDescription.pipelineBindPoint            = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount         = 1;
    subpassDescription.pColorAttachments            = &colorReference;
    subpassDescription.pDepthStencilAttachment      = &depthReference;
    subpassDescription.inputAttachmentCount         = 0;
    subpassDescription.pInputAttachments            = nullptr;
    subpassDescription.preserveAttachmentCount      = 0;
    subpassDescription.pPreserveAttachments         = nullptr;
    subpassDescription.pResolveAttachments          = nullptr;

    array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass      = 0;
    dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass      = 0;
    dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo   = {};
    renderPassInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount          = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments             = attachments.data();
    renderPassInfo.subpassCount             = 1;
    renderPassInfo.pSubpasses               = &subpassDescription;
    renderPassInfo.dependencyCount          = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies            = dependencies.data();

    vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass);
}

/**
 * [RendererVK::CreatePipelineCache description]
 */

void RendererVK::CreatePipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    vkCreatePipelineCache(logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
}

/**
 * [RendererVK::SetupFrameBuffer description]
 */

void RendererVK::SetupFrameBuffer()
{
    VkImageView attachments[2];
    attachments[1] = zView;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType             = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext             = NULL;
    frameBufferCreateInfo.renderPass        = renderPass;
    frameBufferCreateInfo.attachmentCount   = 2;
    frameBufferCreateInfo.pAttachments      = attachments;
    frameBufferCreateInfo.width             = g_settings.winW;
    frameBufferCreateInfo.height            = g_settings.winH;
    frameBufferCreateInfo.layers            = 1;

    frameBuffers.resize(scSize);
    for (uint32_t i = 0; i < frameBuffers.size(); i++)
    {
        attachments[0] = scImageViews[i];
        vkCreateFramebuffer(logicalDevice, &frameBufferCreateInfo, nullptr, &frameBuffers[i]);
    }
}

/**
 * RendererVK::CreateGeometry Create vertex and index buffers for a triangle, upload
 * to GPU through staging buffer.
 */

void RendererVK::CreateGeometry()
{
    // Positions and colors.

    vector<float> vertexBuffer =
    {
       1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
       -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f 
    };
    
    uint32_t vertexBufferSize = 18 * sizeof(float);

    vector<uint32_t> indexBuffer = { 0, 1, 2 };
    idxCnt = static_cast<uint32_t>(indexBuffer.size());
    uint32_t indexBufferSize = 3 * sizeof(uint32_t);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    VkMemoryRequirements memReqs;

    void *data;

    struct StagingBuffer 
    {
        VkDeviceMemory memory;
        VkBuffer buffer;
    };

    struct 
    {
        StagingBuffer vertices;
        StagingBuffer indices;
    } stagingBuffers;

    VkBufferCreateInfo vertexBufferInfo = {};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.size = vertexBufferSize;
    
    vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    
    vkCreateBuffer(logicalDevice, &vertexBufferInfo, nullptr, &stagingBuffers.vertices.buffer);
    vkGetBufferMemoryRequirements(logicalDevice, stagingBuffers.vertices.buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    
    memAlloc.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &stagingBuffers.vertices.memory);
    
    vkMapMemory(logicalDevice, stagingBuffers.vertices.memory, 0, memAlloc.allocationSize, 0, &data);
    memcpy(data, vertexBuffer.data(), vertexBufferSize);
    vkUnmapMemory(logicalDevice, stagingBuffers.vertices.memory);
    vkBindBufferMemory(logicalDevice, stagingBuffers.vertices.buffer, stagingBuffers.vertices.memory, 0);

    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vkCreateBuffer(logicalDevice, &vertexBufferInfo, nullptr, &vertBuf);
    vkGetBufferMemoryRequirements(logicalDevice, vertBuf, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &vertMem);
    vkBindBufferMemory(logicalDevice, vertBuf, vertMem, 0);

    VkBufferCreateInfo indexbufferInfo = {};
    indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexbufferInfo.size = indexBufferSize;
    indexbufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    vkCreateBuffer(logicalDevice, &indexbufferInfo, nullptr, &stagingBuffers.indices.buffer);
    vkGetBufferMemoryRequirements(logicalDevice, stagingBuffers.indices.buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &stagingBuffers.indices.memory);
    vkMapMemory(logicalDevice, stagingBuffers.indices.memory, 0, indexBufferSize, 0, &data);
    memcpy(data, indexBuffer.data(), indexBufferSize);
    vkUnmapMemory(logicalDevice, stagingBuffers.indices.memory);
    vkBindBufferMemory(logicalDevice, stagingBuffers.indices.buffer, stagingBuffers.indices.memory, 0);

    indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vkCreateBuffer(logicalDevice, &indexbufferInfo, nullptr, &idxBuf);
    vkGetBufferMemoryRequirements(logicalDevice, idxBuf, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &idxMem);
    vkBindBufferMemory(logicalDevice, idxBuf, idxMem, 0);

    VkCommandBuffer copyCmd = GetCommandBuffer(true);
    VkBufferCopy copyRegion = {};

    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, stagingBuffers.vertices.buffer, vertBuf, 1, &copyRegion);
    
    copyRegion.size = indexBufferSize;
    vkCmdCopyBuffer(copyCmd, stagingBuffers.indices.buffer, idxBuf, 1, &copyRegion);

    FlushCommandBuffer(copyCmd);

    vkDestroyBuffer(logicalDevice, stagingBuffers.vertices.buffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBuffers.vertices.memory, nullptr);
    vkDestroyBuffer(logicalDevice, stagingBuffers.indices.buffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBuffers.indices.memory, nullptr);
}

/**
 * [RendererVK::GetCommandBuffer description]
 * @param  begin [description]
 * @return       [description]
 */

VkCommandBuffer RendererVK::GetCommandBuffer(bool begin)
{
    VkCommandBuffer cmdBuffer;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = cmdPool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(logicalDevice, &cmdBufAllocateInfo, &cmdBuffer);

    if (begin)
    {
        VkCommandBufferBeginInfo cmdBufInfo = {};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo);
    }

    return cmdBuffer;
}

/**
 * [RendererVK::FlushCommandBuffer description]
 * @param cmdBuf [description]
 */

void RendererVK::FlushCommandBuffer(VkCommandBuffer cmdBuf)
{
    vkEndCommandBuffer(cmdBuf);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuf;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence);

    vkQueueSubmit(queue, 1, &submitInfo, fence);
    vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, 100000000000);

    vkDestroyFence(logicalDevice, fence, nullptr);
    vkFreeCommandBuffers(logicalDevice, cmdPool, 1, &cmdBuf);
}

/**
 * RendererVK::CreateUniformBuffers Initialize triangle uniforms (just MVP matrices in this case).
 */

void RendererVK::CreateUniformBuffers()
{
    VkMemoryRequirements memReqs;
    VkBufferCreateInfo bufferInfo   = {};
    VkMemoryAllocateInfo allocInfo  = {};
    allocInfo.sType                 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext                 = nullptr;
    allocInfo.allocationSize        = 0;
    allocInfo.memoryTypeIndex       = 0;

    bufferInfo.sType                = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size                 = sizeof(TriangleUniforms);
    bufferInfo.usage                = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &ufmBuf);
    vkGetBufferMemoryRequirements(logicalDevice, ufmBuf, &memReqs);
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &ufmMem);

    vkBindBufferMemory(logicalDevice, ufmBuf, ufmMem, 0);

    ufmDesc.buffer = ufmBuf;
    ufmDesc.offset = 0;
    ufmDesc.range = sizeof(TriangleUniforms);

    UpdateUniforms();
}

/**
 * RendererVK::UpdateUniforms Map and memcopy to uniform buffer.
 */

void RendererVK::UpdateUniforms()
{
    uniforms.proj   = perspective(radians(60.0f), (float)g_settings.winW / (float)g_settings.winH, 0.1f, 256.0f);
    uniforms.view   = translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0));
    uniforms.model  = mat4(1.0f);

    uint8_t *pData;
    vkMapMemory(logicalDevice, ufmMem, 0, sizeof(TriangleUniforms), 0, (void **)&pData);
    memcpy(pData, &uniforms, sizeof(TriangleUniforms));
    vkUnmapMemory(logicalDevice, ufmMem);
}

/**
 * [RendererVK::SetupDescriptorPool description]
 */

void RendererVK::SetupDescriptorPool()
{
    VkDescriptorPoolSize typeCounts[1];
    typeCounts[0].type              = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    typeCounts[0].descriptorCount   = 1;

    VkDescriptorPoolCreateInfo descriptorPoolInfo   = {};
    descriptorPoolInfo.sType                        = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext                        = nullptr;
    descriptorPoolInfo.poolSizeCount                = 1;
    descriptorPoolInfo.pPoolSizes                   = typeCounts;
    descriptorPoolInfo.maxSets                      = 1;

    vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, nullptr, &descriptorPool);
}

/**
 * [RendererVK::SetupDescriptorSetLayout description]
 */

void RendererVK::SetupDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding layoutBinding  = {};
    layoutBinding.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount               = 1;
    layoutBinding.stageFlags                    = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers            = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorLayout    = {};
    descriptorLayout.sType                              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext                              = nullptr;
    descriptorLayout.bindingCount                       = 1;
    descriptorLayout.pBindings                          = &layoutBinding;

    vkCreateDescriptorSetLayout(logicalDevice, &descriptorLayout, nullptr, &descriptorSetLayout);

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo    = {};
    pPipelineLayoutCreateInfo.sType                         = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext                         = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount                = 1;
    pPipelineLayoutCreateInfo.pSetLayouts                   = &descriptorSetLayout;

    vkCreatePipelineLayout(logicalDevice, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
}

/**
 * [RendererVK::SetupDescriptorSet description]
 */

void RendererVK::SetupDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo   = {};
    allocInfo.sType                         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool                = descriptorPool;
    allocInfo.descriptorSetCount            = 1;
    allocInfo.pSetLayouts                   = &descriptorSetLayout;

    vkAllocateDescriptorSets(logicalDevice, &allocInfo, &descriptorSet);

    VkWriteDescriptorSet writeDescriptorSet     = {};

    writeDescriptorSet.sType                    = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet                   = descriptorSet;
    writeDescriptorSet.descriptorCount          = 1;
    writeDescriptorSet.descriptorType           = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo              = &ufmDesc;
    writeDescriptorSet.dstBinding               = 0;

    vkUpdateDescriptorSets(logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
}

/**
 * [RendererVK::LoadShader description]
 * @param  file [description]
 * @return      [description]
 */

VkShaderModule RendererVK::LoadShader(string file)
{
    size_t shaderSize;
    char* shaderCode = NULL;
    
    ifstream is(file, ios::binary | ios::in | ios::ate);

    shaderSize = is.tellg();
    is.seekg(0, std::ios::beg);
    shaderCode = new char[shaderSize];
    is.read(shaderCode, shaderSize);
    is.close();

    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = shaderSize;
    moduleCreateInfo.pCode = (uint32_t*)shaderCode;

    VkShaderModule shaderModule;
    vkCreateShaderModule(logicalDevice, &moduleCreateInfo, NULL, &shaderModule);

    delete[] shaderCode;

    return shaderModule;
}

/**
 * [RendererVK::SetupPipelineState description]
 */

void RendererVK::SetupPipelineState()
{
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout       = pipelineLayout;
    pipelineCreateInfo.renderPass   = renderPass;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType        = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology     = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode              = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode                 = VK_CULL_MODE_NONE;
    rasterizationState.frontFace                = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthClampEnable         = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable  = VK_FALSE;
    rasterizationState.depthBiasEnable          = VK_FALSE;
    rasterizationState.lineWidth                = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
    blendAttachmentState[0].colorWriteMask  = 0xf;
    blendAttachmentState[0].blendEnable     = VK_FALSE;
    
    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount     = 1;
    colorBlendState.pAttachments        = blendAttachmentState;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    vector<VkDynamicState> dynamicStateEnables;
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
    
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates     = dynamicStateEnables.data();
    dynamicState.dynamicStateCount  = static_cast<uint32_t>(dynamicStateEnables.size());

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType                     = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable           = VK_TRUE;
    depthStencilState.depthWriteEnable          = VK_TRUE;
    depthStencilState.depthCompareOp            = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable     = VK_FALSE;
    depthStencilState.back.failOp               = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp               = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp            = VK_COMPARE_OP_ALWAYS;
    depthStencilState.stencilTestEnable         = VK_FALSE;
    depthStencilState.front                     = depthStencilState.back;

    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.pSampleMask            = nullptr;

    VkVertexInputBindingDescription vertexInputBinding = {};
    vertexInputBinding.binding      = 0;
    vertexInputBinding.stride       = 6 * sizeof(float);
    vertexInputBinding.inputRate    = VK_VERTEX_INPUT_RATE_VERTEX;

    array<VkVertexInputAttributeDescription, 2> vertexInputAttributs;
    
    vertexInputAttributs[0].binding     = 0;
    vertexInputAttributs[0].location    = 0;
    vertexInputAttributs[0].format      = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[0].offset      = 0;
    
    vertexInputAttributs[1].binding     = 0;
    vertexInputAttributs[1].location    = 1;
    vertexInputAttributs[1].format      = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[1].offset      = 3;

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount      = 1;
    vertexInputState.pVertexBindingDescriptions         = &vertexInputBinding;
    vertexInputState.vertexAttributeDescriptionCount    = 2;
    vertexInputState.pVertexAttributeDescriptions       = vertexInputAttributs.data();

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
    shaderStages[0].sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage   = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module  = LoadShader("shaders/simple.vert.spv");
    shaderStages[0].pName   = "main";

    shaderStages[1].sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module  = LoadShader("shaders/simple.vert.spv");
    shaderStages[1].pName   = "main";

    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    pipelineCreateInfo.pVertexInputState    = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState  = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState  = &rasterizationState;
    pipelineCreateInfo.pColorBlendState     = &colorBlendState;
    pipelineCreateInfo.pMultisampleState    = &multisampleState;
    pipelineCreateInfo.pViewportState       = &viewportState;
    pipelineCreateInfo.pDepthStencilState   = &depthStencilState;
    pipelineCreateInfo.renderPass           = renderPass;
    pipelineCreateInfo.pDynamicState        = &dynamicState;

    vkCreateGraphicsPipelines(logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);

    vkDestroyShaderModule(logicalDevice, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(logicalDevice, shaderStages[1].module, nullptr);
}

/**
 * RendererVK::BuildCommandBuffers
 */

void RendererVK::BuildCommandBuffers()
{
    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    VkClearValue clearValues[2];
    clearValues[0].color        = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType                       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext                       = nullptr;
    renderPassBeginInfo.renderPass                  = renderPass;
    renderPassBeginInfo.renderArea.offset.x         = 0;
    renderPassBeginInfo.renderArea.offset.y         = 0;
    renderPassBeginInfo.renderArea.extent.width     = g_settings.winW;
    renderPassBeginInfo.renderArea.extent.height    = g_settings.winH;
    renderPassBeginInfo.clearValueCount             = 2;
    renderPassBeginInfo.pClearValues                = clearValues;

    for (uint32_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
        renderPassBeginInfo.framebuffer = frameBuffers[i];

        vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo);
        vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport     = {};
        viewport.height         = (float)g_settings.winH;
        viewport.width          = (float)g_settings.winW;
        viewport.minDepth       = (float) 0.0f;
        viewport.maxDepth       = (float) 1.0f;
        
        vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

        VkRect2D scissor        = {};
        scissor.extent.width    = g_settings.winW;
        scissor.extent.height   = g_settings.winH;
        scissor.offset.x        = 0;
        scissor.offset.y        = 0;
        
        vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &vertBuf, offsets);
        vkCmdBindIndexBuffer(drawCmdBuffers[i], idxBuf, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(drawCmdBuffers[i], 3, 1, 0, 0, 1);
        vkCmdEndRenderPass(drawCmdBuffers[i]);

        vkEndCommandBuffer(drawCmdBuffers[i]);
    }
}

/**
 * RendererVK::Render Acquire next swapchain buffer, execute command buffers, and present.
 */

void RendererVK::Render()
{
    vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, presentComplete, (VkFence)nullptr, &curSCBuf);
    vkWaitForFences(logicalDevice, 1, &fences[curSCBuf], VK_TRUE, UINT64_MAX);
    vkResetFences(logicalDevice, 1, &fences[curSCBuf]);

    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask    = &waitStageMask;
    submitInfo.pWaitSemaphores      = &presentComplete;
    submitInfo.waitSemaphoreCount   = 1;																	
    submitInfo.pSignalSemaphores    = &submitComplete;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pCommandBuffers      = &drawCmdBuffers[curSCBuf];
    submitInfo.commandBufferCount   = 1;

    vkQueueSubmit(queue, 1, &submitInfo, fences[curSCBuf]);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext           = NULL;
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = &swapChain;
    presentInfo.pImageIndices   = &curSCBuf;

    if (submitComplete != VK_NULL_HANDLE)
    {
        presentInfo.pWaitSemaphores     = &submitComplete;
        presentInfo.waitSemaphoreCount  = 1;
    }
    
    vkQueuePresentKHR(queue, &presentInfo);
}