#include "utilsvk.h"

map<VkResult, string> errorCodes =
{
    { VK_SUCCESS, "VK_SUCCESS"},
    { VK_NOT_READY, "VK_NOT_READY"},
    { VK_TIMEOUT, "VK_TIMEOUT"},
    { VK_EVENT_SET, "VK_EVENT_SET"},
    { VK_EVENT_RESET, "VK_EVENT_RESET"},
    { VK_INCOMPLETE, "VK_INCOMPLETE"},
    { VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY"},
    { VK_ERROR_OUT_OF_DEVICE_MEMORY, "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
    { VK_ERROR_INITIALIZATION_FAILED, "VK_ERROR_INITIALIZATION_FAILED"},
    { VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST"},
    { VK_ERROR_MEMORY_MAP_FAILED, "VK_ERROR_MEMORY_MAP_FAILED"},
    { VK_ERROR_LAYER_NOT_PRESENT, "VK_ERROR_LAYER_NOT_PRESENT"},
    { VK_ERROR_EXTENSION_NOT_PRESENT, "VK_ERROR_EXTENSION_NOT_PRESENT"},
    { VK_ERROR_FEATURE_NOT_PRESENT, "VK_ERROR_FEATURE_NOT_PRESENT"},
    { VK_ERROR_INCOMPATIBLE_DRIVER, "VK_ERROR_INCOMPATIBLE_DRIVER"},
    { VK_ERROR_TOO_MANY_OBJECTS, "VK_ERROR_TOO_MANY_OBJECTS"},
    { VK_ERROR_FORMAT_NOT_SUPPORTED, "VK_ERROR_FORMAT_NOT_SUPPORTED"},
    { VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL"},
    { VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY"},
    { VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
    { VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR"},
    { VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
    { VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR"},
    { VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR"},
    { VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
    { VK_ERROR_VALIDATION_FAILED_EXT, "VK_ERROR_VALIDATION_FAILED_EXT"},
    { VK_ERROR_INVALID_SHADER_NV, "VK_ERROR_INVALID_SHADER_NV"},
    { VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"},
    { VK_ERROR_FRAGMENTATION_EXT, "VK_ERROR_FRAGMENTATION_EXT"},
    { VK_ERROR_NOT_PERMITTED_EXT, "VK_ERROR_NOT_PERMITTED_EXT"},
    { VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "VK_ERROR_OUT_OF_POOL_MEMORY_KHR"},
    { VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR, "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR"},
    { VK_RESULT_BEGIN_RANGE, "VK_RESULT_BEGIN_RANGE"},
    { VK_RESULT_END_RANGE, "VK_RESULT_END_RANGE"},
    { VK_RESULT_RANGE_SIZE, "VK_RESULT_RANGE_SIZE"},
    { VK_RESULT_MAX_ENUM, "VK_RESULT_MAX_ENUM"}
};

void CHECK_RESULT(VkResult res)
{
#ifdef _DEBUG
    if (res != VK_SUCCESS)
    {
        printf("Vk error: %s\n", errorCodes[res].c_str());
        __debugbreak();
    }
#endif
}

VkShaderModule LoadShader(VkDevice &logicalDevice, string file)
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
    CHECK_RESULT(vkCreateShaderModule(logicalDevice, &moduleCreateInfo, NULL, &shaderModule));

    delete[] shaderCode;

    return shaderModule;
}

/**
 * FindProperties Select memory heap from list available device heaps based on
 * required memory properties.
 *
 * @param  memoryTypeBitsRequirement Required memory bits [in].
 * @param  requiredProperties        Required memory properties [in].
 * @param  deviceMemoryProperties    List of available memory heaps [in].
 *
 * @return                           Index of heap with desired properties if found, ~0 otherwise.
 */

uint32_t FindProperties(
    uint32_t memoryTypeBitsRequirement,
    VkMemoryPropertyFlags requiredProperties,
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties
)
{
    const uint32_t memoryCount = deviceMemoryProperties.memoryTypeCount;

    for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; memoryIndex++)
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