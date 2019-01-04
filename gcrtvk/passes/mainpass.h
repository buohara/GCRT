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

	VkDeviceMemory idxMem;
	VkBuffer idxBuf;
	uint32_t idxCnt;

	VkDeviceMemory vertMem;
	VkBuffer vertBuf;

	VkDeviceMemory vertStgMem;
	VkBuffer vertStgBuf;

	VkDeviceMemory idxStgMem;
	VkBuffer idxStgBuf;

	mat4 proj;
	mat4 model;
	mat4 view;

	void CreateGeometry(VkDevice &logicalDevice);

	/**
	* Get rid of this.
	*/

	void CreateRenderPass(VkDevice &logicalDevice);
	void CreateDepth(VkDevice &logicalDevice);
	void CreatePipelineCache(VkDevice &logicalDevice);
	void SetupFrameBuffer(VkDevice &logicalDevice);
	void CreateUniformBuffers(VkDevice &logicalDevice);
	void UpdateUniforms(VkDevice &logicalDevice);
	void SetupDescriptorPool(VkDevice &logicalDevice);
	void SetupDescriptorSetLayout(VkDevice &logicalDevice);
	void SetupDescriptorSet(VkDevice &logicalDevice);
	void SetupPipelineState(VkDevice &logicalDevice);
	void BuildCommandBuffers(VkDevice &logicalDevice);
};