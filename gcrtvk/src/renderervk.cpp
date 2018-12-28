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
    bool useSwapChain = true,
    VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)
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

void RendererVK::GetPresentSurface(HINSTANCE hInstance)
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
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);

    vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    VkExtent2D swapchainExtent = {};

    if (surfCaps.currentExtent.width == (uint32_t)-1)
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

    vkCreateSwapchainKHR(logicalDevice, &swapchainCI, nullptr, &swapChain);

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        for (uint32_t i = 0; i < imageCount; i++)
        {
            vkDestroyImageView(logicalDevice, imageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
    }

    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, NULL);

    images.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, images.data());

    imageViews.resize(imageCount);
    
    for (uint32_t i = 0; i < imageCount; i++)
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

        colorAttachmentView.image = images[i];

        vkCreateImageView(logicalDevice, &colorAttachmentView, nullptr, &imageViews[i]);
    }
}

/**
 * [RendererVK::CreateCommandBuffers description]
 */

void RendererVK::CreateCommandBuffers()
{
    drawCmdBuffers.resize(imageCount);

    VkCommandBufferAllocateInfo cmdBufferAllocateInfo   = {};
    cmdBufferAllocateInfo.sType                         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocateInfo.commandPool                   = cmdPool;
    cmdBufferAllocateInfo.level                         = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocateInfo.commandBufferCount            = imageCount;

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
    mem_alloc.memoryTypeIndex = FindProperties()
    vkAllocateMemory(logicalDevice, &mem_alloc, nullptr, &depthStencil.mem);
    vkBindImageMemory(logicalDevice, depthStencil.image, depthStencil.mem, 0);

    depthStencilView.image = depthStencil.image;
    vkCreateImageView(logicalDevice, &depthStencilView, nullptr, &depthStencil.view);
}

/**
 * [RendererVK::CreateRenderPass description]
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