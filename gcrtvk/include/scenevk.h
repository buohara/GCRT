#pragma once

#include "gcrtvk.h"
#include "meshvk.h"

using namespace std;

struct SceneVk
{
    vector<MeshVk> meshes;

    VkCommandBuffer uploadBuf;
    VkFence uploadFence;
    VkCommandPool uploadCmdPool;

    void Add(MeshVk& mesh) { meshes.push_back(mesh); }

    SceneVk(VkDevice& logicalDevice, uint32_t queueIdx);

    void RecordUploadScene(
        VkDevice& logicalDevice,
        VkPhysicalDeviceMemoryProperties& deviceMemoryProperties
    )
    {
        for (auto& mesh : meshes) mesh.RecordUpload(logicalDevice, deviceMemoryProperties, uploadBuf);
    }
};