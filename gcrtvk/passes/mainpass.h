#pragma once

#include "renderpass.h"

using namespace glm;
using namespace std;

struct TriangleUniforms
{
	mat4 proj;
	mat4 model;
	mat4 view;
};

struct MainPassVk : RenderPassVk
{
	VkFormat colorFormat;
	VkFormat depthFormat;

	VkDeviceMemory ufmMem;
	VkBuffer ufmBuf;
	VkDescriptorBufferInfo ufmDesc;

	VkRenderPass renderPass;

	TriangleUniforms uniforms;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkPipelineCache pipelineCache;

	/**
	* Get rid of this.
	*/

	mat4 proj;
	mat4 model;
	mat4 view;

	/**
	* Get rid of this.
	*/

    void Init(VkDevice &logicalDevice, bool renderToOutput);

	void CreateRenderPass(VkDevice &logicalDevice);
	void CreatePipelineCache(VkDevice &logicalDevice);
	void CreateUniformBuffers(VkDevice &logicalDevice);
	void UpdateUniforms(VkDevice &logicalDevice);
	void SetupDescriptorPool(VkDevice &logicalDevice);
	void SetupDescriptorSetLayout(VkDevice &logicalDevice);
	void SetupDescriptorSet(VkDevice &logicalDevice);
	void SetupPipelineState(VkDevice &logicalDevice);
	void BuildCommandBuffers(VkDevice &logicalDevice, VkCommandBuffer &cmdBuf);

    VkRenderPass GetRenderPass();
};