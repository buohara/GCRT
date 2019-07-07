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
    uint32_t graphicsQueueIdx;

    VkDeviceMemory ufmMem;
    VkBuffer ufmBuf;
    VkDescriptorBufferInfo ufmDesc;

    VkRenderPass renderPass;

    vector<VkCommandBuffer> cmdBuffers;
    vector<VkFence> fences;

    TriangleUniforms uniforms;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineCache pipelineCache;
    vector<VkFramebuffer> frameBuffers;

    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

    bool renderToFrameBuffer;
    uint32_t scSize;

    VkCommandPool cmdPool;

    VkImage zImage;
    VkDeviceMemory zMem;
    VkImageView zView;

    RenderPassVk(
        VkDevice& logicalDevice,
        VkPhysicalDeviceMemoryProperties &deviceMemoryProperties,
        uint32_t graphicsQueueIdx,
        bool renderToOutput,
        VkFormat colorFormat,
        VkFormat depthFormat,
        uint32_t scSize,
        vector<VkImageView>& scViews
    );

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
    void CreateCommandPool(VkDevice& logicalDevice);
    void SetupFrameBuffer(VkDevice& logicalDevice, vector<VkImageView>& scViews);
    void CreateDepth(VkDevice& logicalDevice);

    VkRenderPass GetRenderPass();
};