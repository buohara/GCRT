#include "scenevk.h"

SceneVk::SceneVk(VkDevice& logicalDevice, uint32_t queueIdx)
{
    // upload command pool

    VkCommandPoolCreateInfo cmdPoolInfo = {};

    cmdPoolInfo.sType               = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex    = queueIdx;
    cmdPoolInfo.flags               = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &uploadCmdPool);

    // upload command buffer

    VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {};

    cmdBufferAllocateInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocateInfo.commandPool           = uploadCmdPool;
    cmdBufferAllocateInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocateInfo.commandBufferCount    = 1;

    vkAllocateCommandBuffers(logicalDevice, &cmdBufferAllocateInfo, &uploadBuf);

    // upload fence

    VkFenceCreateInfo fenceCreateInfo   = {};
    fenceCreateInfo.sType               = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &uploadFence);
}

void SceneVk::RecordUploadScene(
    VkDevice& logicalDevice,
    VkPhysicalDeviceMemoryProperties& deviceMemoryProperties
)
{
    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    vkBeginCommandBuffer(uploadBuf, &cmdBufInfo);
    for (auto& mesh : meshes) mesh.RecordUpload(logicalDevice, deviceMemoryProperties, uploadBuf);
    vkEndCommandBuffer(uploadBuf);
}