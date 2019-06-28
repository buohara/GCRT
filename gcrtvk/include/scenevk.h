#pragma once

#include "gcrtvk.h"
#include "meshvk.h"

using namespace std;

struct SceneVk
{
    vector<MeshVk> meshes;
    
    void RecordUploadScene(
        VkDevice& logicalDevice,
        VkPhysicalDeviceMemoryProperties& deviceMemoryProperties,
        VkCommandBuffer& buf
    )
    {
        for (auto& mesh : meshes) mesh.RecordUpload(logicalDevice, deviceMemoryProperties, buf);
    }
};