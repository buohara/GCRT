#include "meshvk.h"

MeshVk::MeshVk(VkDevice& logicalDevice, VkPhysicalDeviceMemoryProperties& deviceMemoryProperties, MeshTypeVk type) : type(type)
{
    assert(type == TRIANGLE);

    pos.push_back({ -1.0, -1.0, 0.0 });
    pos.push_back({ 0.0, 1.0, 0.0 });
    pos.push_back({ 1.0, -1.0, 0.0 });

    norm.push_back({ 0.0, 0.0, 1.0 });
    norm.push_back({ 0.0, 0.0, 1.0 });
    norm.push_back({ 0.0, 0.0, 1.0 });

    uv.push_back({ 0.0, 0.0 });
    uv.push_back({ 0.5, 1.0 });
    uv.push_back({ 1.0, 0.0 });
}

void MeshVk::RecordUpload(VkDevice& logicalDevice, VkPhysicalDeviceMemoryProperties& deviceMemoryProperties, VkCommandBuffer &buf)
{
    switch (type)
    {
    case TRIANGLE:

        InitVertexObjectsVk(pos, norm, uv, logicalDevice, deviceMemoryProperties, buf);
        break;
    
    default:
        break;
    }
}

/**
 * Mesh::UploadThroughStageVk Upload a buffer to GPU memory through a stg(CPU)->buf(GPU)
 * copy.
 *
 * @param logicalDevice Vulkan device for VK operations.
 * @param buf           GPU buffer handle to upload to.
 * @param mem           GPU memory handle for GPU buffer.
 * @param data          Buffer data to upload.
 * @param uploadCmdBuf  Command buffer to record upload into.
 */

void MeshVk::UploadThroughStageVk(
    VkDevice& logicalDevice,
    VkBuffer& buf,
    VkDeviceMemory& mem,
    VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
    void* data,
    uint32_t size,
    VkCommandBuffer& uploadCmdBuf
)
{
    VkDeviceMemory stgMem;
    VkBuffer stgBuf;

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    VkMemoryRequirements memReqs;

    VkBufferCreateInfo vertexBufferInfo = {};
    vertexBufferInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.size   = size;
    vertexBufferInfo.usage  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    // Create staging buffer. Map and copy data to it.

    vkCreateBuffer(logicalDevice, &vertexBufferInfo, nullptr, &stgBuf);
    vkGetBufferMemoryRequirements(logicalDevice, stgBuf, &memReqs);
    memAlloc.allocationSize = memReqs.size;

    memAlloc.memoryTypeIndex = FindProperties(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        deviceMemoryProperties
    );

    vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &stgMem);

    void* pData;

    vkMapMemory(logicalDevice, stgMem, 0, memAlloc.allocationSize, 0, &pData);
    memcpy(pData, data, size);
    vkUnmapMemory(logicalDevice, stgMem);
    vkBindBufferMemory(logicalDevice, stgBuf, stgMem, 0);

    // Create GPU buffer.

    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vkCreateBuffer(logicalDevice, &vertexBufferInfo, nullptr, &buf);
    vkGetBufferMemoryRequirements(logicalDevice, buf, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    
    memAlloc.memoryTypeIndex = FindProperties(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        deviceMemoryProperties
    );
    
    vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &mem);
    vkBindBufferMemory(logicalDevice, buf, mem, 0);

    // Program the upload and cleanup staging buffer.

    VkBufferCopy copyRegion = {};

    copyRegion.size = size;
    vkCmdCopyBuffer(uploadCmdBuf, stgBuf, buf, 1, &copyRegion);
}

/**
 * Mesh::InitVertexObjectsVk Upload VK vertex objects for a given mesh.
 *
 * @param subMeshIdx             Submesh index of current geometry [in].
 * @param pos                    Input vertex positions [in].
 * @param norms                  Input vertex normals [in].
 * @param uvs                    Input vertex UVs [in].
 * @param boneIDs                Input bones that influence each vertex [in].
 * @param boneWeights            Input weights of vertex's influencing bones [in].
 * @param logicalDevice          VK device for VK operations [in].
 * @param deviceMemoryProperties Memory heap properties for allocating vertex memory [in].
 * @param cmdBuf                 Command buffer to record uploads [in].
 */

void MeshVk::InitVertexObjectsVk(
    vector<vec3>& pos,
    vector<vec3>& norms,
    vector<vec2>& uvs,
    VkDevice& logicalDevice,
    VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
    VkCommandBuffer& cmdBuf
)
{
    UploadThroughStageVk(
        logicalDevice,
        posBuf,
        posMem,
        deviceMemoryProperties,
        (void*)& pos[0],
        (uint32_t)3 * pos.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        normBuf,
        normMem,
        deviceMemoryProperties,
        (void*)& norms[0],
        (uint32_t)3 * norms.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        uvBuf,
        uvMem,
        deviceMemoryProperties,
        (void*)& uvs[0],
        (uint32_t)2 * uvs.size() * sizeof(float),
        cmdBuf
    );
}

/**
 * Mesh::NotifyUploadCompleteVk Notify mesh that geometry has been uploaded so
 * stating resources can be released.
 *
 * @param logicalDevice Device used to release resources.
 */

void MeshVk::NotifyUploadCompleteVk(VkDevice& logicalDevice)
{
    ReleaseStageMemVk(logicalDevice);
}

/**
 * SubMesh::ReleaseStageMemVk After vertex data uploaded, release any
 * used staging resources.
 *
 * @param logicalDevice Device used to release stage mem.
 */

void MeshVk::ReleaseStageMemVk(VkDevice& logicalDevice)
{
    if (posStgBuf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(logicalDevice, posStgBuf, nullptr);
        vkFreeMemory(logicalDevice, posStgMem, nullptr);
    }

    if (normStgBuf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(logicalDevice, normStgBuf, nullptr);
        vkFreeMemory(logicalDevice, normStgMem, nullptr);
    }

    if (uvStgBuf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(logicalDevice, uvStgBuf, nullptr);
        vkFreeMemory(logicalDevice, uvStgMem, nullptr);
    }

    if (tanStgBuf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(logicalDevice, tanStgBuf, nullptr);
        vkFreeMemory(logicalDevice, tanStgMem, nullptr);
    }

    if (idxStgBuf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(logicalDevice, idxStgBuf, nullptr);
        vkFreeMemory(logicalDevice, idxStgMem, nullptr);
    }
}