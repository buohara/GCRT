#include "Mesh.h"

/**
 * Mesh::InitVertexObjects Create GL buffers/vertex attributes for a mesh.
 *
 * @param subMeshIdx  Mesh submesh index (for composite meshes).
 * @param pos         Vertex positions.
 * @param norms       Vertex normals.
 * @param uvs         Vertex UVs.
 * @param tans        Vertex tangents.
 * @param boneIDs     Vertex bones.
 * @param boneWeights Vertex bone weights.
 */

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos, 
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<vec3> &tans,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts = (uint32_t)pos.size();

    size_t posBufSize    = 3 * numVerts * sizeof(float);
    size_t normBufSize   = 3 * numVerts * sizeof(float);
    size_t uvBufSize     = 2 * numVerts * sizeof(float);
    size_t tanBufSize    = 3 * numVerts * sizeof(float);
    size_t boneIDBufSize = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].tanVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].tanVboID);
    glBufferData(GL_ARRAY_BUFFER, tanBufSize, &tans[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
}

/**
 * Mesh::InitVertexObjects Create GL buffers/vertex attributes for a mesh.
 *
 * @param subMeshIdx  Mesh submesh index (for composite meshes).
 * @param pos         Vertex positions.
 * @param norms       Vertex normals.
 * @param uvs         Vertex UVs.
 * @param boneIDs     Vertex bones.
 * @param boneWeights Vertex bone weights.
 */

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts = (uint32_t)pos.size();

    size_t posBufSize    = 3 * numVerts * sizeof(float);
    size_t normBufSize   = 3 * numVerts * sizeof(float);
    size_t uvBufSize     = 2 * numVerts * sizeof(float);
    size_t boneIDBufSize = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
}

/**
* Mesh::InitVertexObjects Create GL buffers/vertex attributes for a mesh.
*
* @param subMeshIdx  Mesh submesh index (for composite meshes).
* @param pos         Vertex positions.
* @param norms       Vertex normals.
* @param uvs         Vertex UVs.
* @param idcs        Triangle vertex indices.
* @param boneIDs     Vertex bones.
* @param boneWeights Vertex bone weights.
*/

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<uint32_t> &idcs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts       = (uint32_t)pos.size();
    uint32_t numIdcs        = (uint32_t)idcs.size();

    size_t posBufSize       = 3 * numVerts * sizeof(float);
    size_t normBufSize      = 3 * numVerts * sizeof(float);
    size_t uvBufSize        = 2 * numVerts * sizeof(float);
    size_t idxBufSize       = numIdcs * sizeof(uint32_t);
    size_t boneIDBufSize    = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize    = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glGenBuffers(1, &subMeshes[subMeshIdx].idxVboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[subMeshIdx].idxVboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBufSize, &idcs[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
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

void Mesh::UploadThroughStageVk(
    VkDevice &logicalDevice,
    VkBuffer &buf,
    VkDeviceMemory &mem,
    VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
    void *data,
    uint32_t size,
    VkCommandBuffer &uploadCmdBuf
)
{
    VkDeviceMemory stgMem;
    VkBuffer stgBuf;

    VkMemoryAllocateInfo memAlloc           = {};
    memAlloc.sType                          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    VkMemoryRequirements memReqs;

    VkBufferCreateInfo vertexBufferInfo     = {};
    vertexBufferInfo.sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.size                   = size;
    vertexBufferInfo.usage                  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

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

    void *pData;

    vkMapMemory(logicalDevice, stgMem, 0, memAlloc.allocationSize, 0, &pData);
    memcpy(pData, data, size);
    vkUnmapMemory(logicalDevice, stgMem);
    vkBindBufferMemory(logicalDevice, stgBuf, stgMem, 0);

    // Create GPU buffer.

    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vkCreateBuffer(logicalDevice, &vertexBufferInfo, nullptr, &buf);
    vkGetBufferMemoryRequirements(logicalDevice, buf, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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

void Mesh::InitVertexObjectsVk(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights,
    VkDevice &logicalDevice,
    VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
    VkCommandBuffer &cmdBuf
    )
{
    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&pos[0],
        3 * pos.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].normBuf,
        subMeshes[subMeshIdx].normMem,
        deviceMemoryProperties,
        (void*)&norms[0],
        3 * norms.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].uvBuf,
        subMeshes[subMeshIdx].uvMem,
        deviceMemoryProperties,
        (void*)&uvs[0],
        2 * uvs.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&boneIDs[0],
        4 * boneIDs.size() * sizeof(int),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&boneWeights[0],
        4 * boneWeights.size() * sizeof(float),
        cmdBuf
    );
}

/**
 * Mesh::InitVertexObjectsVk Upload VK vertex objects for a given mesh.
 * 
 * @param subMeshIdx             Submesh index of current geometry [in].
 * @param pos                    Input vertex positions [in].
 * @param norms                  Input vertex normals [in].
 * @param uvs                    Input vertex UVs [in].
 * @param tans                   Input vertex tangents [in].
 * @param boneIDs                Input bones that influence each vertex [in].
 * @param boneWeights            Input weights of vertex's influencing bones [in].
 * @param logicalDevice          VK device for VK operations [in].
 * @param deviceMemoryProperties Memory heap properties for allocating vertex memory [in]
 * @param cmdBuf                 Command buffer to record uploads [in].
 */

void Mesh::InitVertexObjectsVk(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<vec3> &tans,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights,
    VkDevice &logicalDevice,
    VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
    VkCommandBuffer &cmdBuf
)
{
    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&pos[0],
        3 * pos.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].normBuf,
        subMeshes[subMeshIdx].normMem,
        deviceMemoryProperties,
        (void*)&norms[0],
        3 * norms.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].normBuf,
        subMeshes[subMeshIdx].normMem,
        deviceMemoryProperties,
        (void*)&tans[0],
        3 * tans.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].uvBuf,
        subMeshes[subMeshIdx].uvMem,
        deviceMemoryProperties,
        (void*)&uvs[0],
        2 * uvs.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&boneIDs[0],
        4 * boneIDs.size() * sizeof(int),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&boneWeights[0],
        4 * boneWeights.size() * sizeof(float),
        cmdBuf
    );
}

/**
 * Mesh::InitVertexObjectsVk Upload VK vertex objects for a given mesh.
 * 
 * @param subMeshIdx             Submesh index of current geometry [in].
 * @param pos                    Input vertex positions [in].
 * @param norms                  Input vertex normals [in].
 * @param uvs                    Input vertex UVs [in].
 * @param uvs                    Input triangle vertex indices [in].
 * @param boneIDs                Input bones that influence each vertex [in].
 * @param boneWeights            Input weights of vertex's influencing bones [in].
 * @param logicalDevice          VK device for VK operations [in].
 * @param deviceMemoryProperties Memory heap properties for allocating vertex memory [in].
 * @param cmdBuf                 Command buffer to record uploads [in].
 */

void Mesh::InitVertexObjectsVk(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<uint32_t> &idcs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights,
    VkDevice &logicalDevice,
    VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
    VkCommandBuffer &cmdBuf
)
{
    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&pos[0],
        3 * pos.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].normBuf,
        subMeshes[subMeshIdx].normMem,
        deviceMemoryProperties,
        (void*)&norms[0],
        3 * norms.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].uvBuf,
        subMeshes[subMeshIdx].uvMem,
        deviceMemoryProperties,
        (void*)&uvs[0],
        2 * uvs.size() * sizeof(float),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].uvBuf,
        subMeshes[subMeshIdx].uvMem,
        deviceMemoryProperties,
        (void*)&idcs[0],
        3 * idcs.size() * sizeof(uint32_t),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&boneIDs[0],
        4 * boneIDs.size() * sizeof(int),
        cmdBuf
    );

    UploadThroughStageVk(
        logicalDevice,
        subMeshes[subMeshIdx].posBuf,
        subMeshes[subMeshIdx].posMem,
        deviceMemoryProperties,
        (void*)&boneWeights[0],
        4 * boneWeights.size() * sizeof(float),
        cmdBuf
    );
}

/**
 * Mesh::NotifyUploadCompleteVk Notify mesh that geometry has been uploaded so
 * stating resources can be released.
 * 
 * @param logicalDevice Device used to release resources.
 */

void Mesh::NotifyUploadCompleteVk(VkDevice &logicalDevice)
{
    for (auto& submesh : subMeshes)
    {
        submesh.ReleaseStageMemVk(logicalDevice);
    }
}

/**
 * SubMesh::ReleaseStageMemVk After vertex data uploaded, release any
 * used staging resources.
 *
 * @param logicalDevice Device used to release stage mem.
 */

void SubMesh::ReleaseStageMemVk(VkDevice &logicalDevice)
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