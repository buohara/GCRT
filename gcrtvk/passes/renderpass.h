#pragma once

#include "gcrtvk.h"
#include "utilsvk.h"
#include "scenevk.h"

using namespace glm;

struct TriangleUniforms
{
    mat4 proj;
    mat4 model;
    mat4 view;
};

struct RenderPassVk
{
    VkFormat colorFormat;
    VkFormat depthFormat;

    VkDeviceMemory ufmMem;
    VkBuffer ufmBuf;
    VkDescriptorBufferInfo ufmDesc;

    VkRenderPass renderPass;

    vector<VkCommandBuffer> cmdBuffers;
    vector<VkFramebuffer> frameBuffers;
    vector<VkFence> fences;

    TriangleUniforms uniforms;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkPipelineCache pipelineCache;

    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

    bool renderToFrameBuffer;
    uint32_t scSize;

    VkCommandPool cmdPool;

    void Init(VkDevice& logicalDevice, bool renderToOutput);
    void CreateRenderPass(VkDevice& logicalDevice);
    void CreatePipelineCache(VkDevice& logicalDevice);
    void CreateUniformBuffers(VkDevice& logicalDevice);
    void UpdateUniforms(VkDevice& logicalDevice);
    void CreateCommandBuffers(VkDevice& logicalDevice);
    void CreateFenceObjects(VkDevice& logicalDevice);
    void SetupDescriptorPool(VkDevice& logicalDevice);
    void SetupDescriptorSetLayout(VkDevice& logicalDevice);
    void SetupDescriptorSet(VkDevice& logicalDevice);
    void SetupPipelineState(VkDevice& logicalDevice);
    void BuildCommandBuffers(VkDevice& logicalDevice, uint32_t curSCBuf, SceneVk &scn);
    void CreateCommandPool(VkDevice& logicalDevice, uint32_t queueIdx);
    VkCommandBuffer GetCommandBuffer(bool begin);
    void FlushCommandBuffer(VkCommandBuffer cmdBuf);

    VkRenderPass GetRenderPass();
};