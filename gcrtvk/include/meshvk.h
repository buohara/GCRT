#pragma once

#include "gcrtvk.h"
#include "utilsvk.h"

using namespace std;
using namespace glm;

enum MeshTypeVk
{
    TRIANGLE
};

struct MeshVk
{
    MeshVk(VkDevice& logicalDevice, VkPhysicalDeviceMemoryProperties& deviceMemoryProperties, MeshTypeVk type);
    void RecordUpload(VkDevice& logicalDevice, VkPhysicalDeviceMemoryProperties& deviceMemoryProperties, VkCommandBuffer& buf);

    MeshTypeVk type;

private:

    vector<vec3> pos;
    vector<vec3> norm;
    vector<vec2> uv;

    VkBuffer posBuf             = VK_NULL_HANDLE;
    VkBuffer normBuf            = VK_NULL_HANDLE;
    VkBuffer uvBuf              = VK_NULL_HANDLE;
    VkBuffer tanBuf             = VK_NULL_HANDLE;
    VkBuffer idxBuf             = VK_NULL_HANDLE;

    VkDeviceMemory posMem       = VK_NULL_HANDLE;
    VkDeviceMemory normMem      = VK_NULL_HANDLE;
    VkDeviceMemory uvMem        = VK_NULL_HANDLE;
    VkDeviceMemory tanMem       = VK_NULL_HANDLE;
    VkDeviceMemory idxMem       = VK_NULL_HANDLE;

    VkBuffer posStgBuf          = VK_NULL_HANDLE;
    VkBuffer normStgBuf         = VK_NULL_HANDLE;
    VkBuffer uvStgBuf           = VK_NULL_HANDLE;
    VkBuffer tanStgBuf          = VK_NULL_HANDLE;
    VkBuffer idxStgBuf          = VK_NULL_HANDLE;

    VkDeviceMemory posStgMem    = VK_NULL_HANDLE;
    VkDeviceMemory normStgMem   = VK_NULL_HANDLE;
    VkDeviceMemory uvStgMem     = VK_NULL_HANDLE;
    VkDeviceMemory tanStgMem    = VK_NULL_HANDLE;
    VkDeviceMemory idxStgMem    = VK_NULL_HANDLE;

    void UploadThroughStageVk(
        VkDevice& logicalDevice,
        VkBuffer& buf,
        VkDeviceMemory& mem,
        VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
        void* data,
        uint32_t size,
        VkCommandBuffer& uploadCmdBuf
    );

    void InitVertexObjectsVk(
        vector<vec3>& pos,
        vector<vec3>& norms,
        vector<vec2>& uvs,
        VkDevice& logicalDevice,
        VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
        VkCommandBuffer& cmdBuf
    );

    void ReleaseStageMemVk(VkDevice& logicalDevice);
    void NotifyUploadCompleteVk(VkDevice& logicalDevice);
};