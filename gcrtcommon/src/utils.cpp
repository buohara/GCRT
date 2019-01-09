#include "utils.h"

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