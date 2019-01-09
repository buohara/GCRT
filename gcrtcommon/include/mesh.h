#pragma once

#include "GCRT.h"
#include "utils.h"

using namespace std;
using namespace glm;

struct SubMesh
{
    uint32_t numIdcs;
    uint32_t numVerts;

    // OpenGL vertex buffer handles.

    GLuint vaoID;
    
    GLuint posVboID;
    GLuint normVboID;
    GLuint uvVboID;
    GLuint tanVboID;
    GLuint idxVboID;
    GLuint boneIDVboID;
    GLuint boneWtVboID;

    // Vulkan buffer objects

    VkBuffer posBuf         = VK_NULL_HANDLE;
    VkBuffer normBuf        = VK_NULL_HANDLE;
    VkBuffer uvBuf          = VK_NULL_HANDLE;
    VkBuffer tanBuf         = VK_NULL_HANDLE;
    VkBuffer idxBuf         = VK_NULL_HANDLE;

    VkDeviceMemory posMem   = VK_NULL_HANDLE;
    VkDeviceMemory normMem  = VK_NULL_HANDLE;
    VkDeviceMemory uvMem    = VK_NULL_HANDLE;
    VkDeviceMemory tanMem   = VK_NULL_HANDLE;
    VkDeviceMemory idxMem   = VK_NULL_HANDLE;

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

    void ReleaseStageMemVk(VkDevice &logicalDevice);
};

struct Mesh
{
    vector<SubMesh> subMeshes;
    string name;
    bool loadFromFile;
    bool animated;
    string filePath;
    bool invert = false;

    virtual void Draw() = 0;
    
    virtual void GetAnimation(float t, mat4 rootTrans, vector<mat4> &bones) 
    {
        if (bones.size() != 1)
        {
            bones.resize(1);
        }

        bones[0] = rootTrans;
    };

    // OpenGL vertex buffer creation routines.

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos, 
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<vec3> &tans,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<uint32_t> &idcs,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );

    // Vulkan vertex buffer creation routines.

    void UploadThroughStageVk(
        VkDevice &logicalDevice,
        VkBuffer &buf, 
        VkDeviceMemory &mem,
        VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
        void *data,
        uint32_t size,
        VkCommandBuffer &uploadCmdBuf
    );

    void InitVertexObjectsVk(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights,
        VkDevice &logicalDevice,
        VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
        VkCommandBuffer &cmdBuf
    );

    void InitVertexObjectsVk(
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
    );

    void InitVertexObjectsVk(
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
    );

    void NotifyUploadCompleteVk(VkDevice &logicalDevice);
};