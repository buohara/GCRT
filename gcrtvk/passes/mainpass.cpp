#include "renderpass.h"

extern RenderSettingsVK g_settings;

/**
 * RenderPassVk::Init Initialize the main render pass.
 *
 * @param renderToOutput True if this pass should render to the default
 * (output shows up on screen).
 */

void RenderPassVk::Init(VkDevice &logicalDevice, bool renderToOutput)
{
    renderToFrameBuffer = renderToOutput;

    CreateRenderPass(logicalDevice);
    CreatePipelineCache(logicalDevice);
    CreateUniformBuffers(logicalDevice);
    SetupDescriptorPool(logicalDevice);
    SetupDescriptorSetLayout(logicalDevice);
    SetupDescriptorSet(logicalDevice);
    SetupPipelineState(logicalDevice);
    BuildCommandBuffers(logicalDevice);
}

/**
 * RenderPassVk::CreateRenderPass Setup global renderpass for writing to frame buffer.
 */

void RenderPassVk::CreateRenderPass(VkDevice &logicalDevice)
{
    array<VkAttachmentDescription, 2> attachments = {};

    attachments[0].format           = colorFormat;
    attachments[0].samples          = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachments[1].format           = depthFormat;
    attachments[1].samples          = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout      = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription     = {};
    subpassDescription.pipelineBindPoint        = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount     = 1;
    subpassDescription.pColorAttachments        = &colorReference;
    subpassDescription.pDepthStencilAttachment  = &depthReference;
    subpassDescription.inputAttachmentCount     = 0;
    subpassDescription.pInputAttachments        = nullptr;
    subpassDescription.preserveAttachmentCount  = 0;
    subpassDescription.pPreserveAttachments     = nullptr;
    subpassDescription.pResolveAttachments      = nullptr;

    array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass      = 0;
    dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass      = 0;
    dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};

    renderPassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount  = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments     = attachments.data();
    renderPassInfo.subpassCount     = 1;
    renderPassInfo.pSubpasses       = &subpassDescription;
    renderPassInfo.dependencyCount  = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies    = dependencies.data();

    vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass);
    }

/**
* [RenderPassVk::CreatePipelineCache description]
*/

void RenderPassVk::CreatePipelineCache(VkDevice &logicalDevice)
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    vkCreatePipelineCache(logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
}

/**
* RenderPassVk::CreateUniformBuffers Initialize triangle uniforms (just MVP matrices in this case).
*/

void RenderPassVk::CreateUniformBuffers(VkDevice &logicalDevice)
{
    VkMemoryRequirements memReqs;
    VkBufferCreateInfo bufferInfo = {};
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = 0;
    allocInfo.memoryTypeIndex = 0;

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(TriangleUniforms);
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &ufmBuf);
    vkGetBufferMemoryRequirements(logicalDevice, ufmBuf, &memReqs);
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = FindProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &ufmMem);

    vkBindBufferMemory(logicalDevice, ufmBuf, ufmMem, 0);

    ufmDesc.buffer = ufmBuf;
    ufmDesc.offset = 0;
    ufmDesc.range = sizeof(TriangleUniforms);

    UpdateUniforms();
}

/**
* RenderPassVk::UpdateUniforms Map and memcopy to uniform buffer.
*/

void RenderPassVk::UpdateUniforms(VkDevice &logicalDevice)
{
    uniforms.proj = perspective(radians(60.0f), (float)g_settings.winW / (float)g_settings.winH, 0.1f, 256.0f);
    uniforms.view = translate(vec3(0.0, 0.0, -1.0));
    uniforms.model = mat4(1.0f);

    uint8_t *pData;
    vkMapMemory(logicalDevice, ufmMem, 0, sizeof(TriangleUniforms), 0, (void **)&pData);
    memcpy(pData, &uniforms, sizeof(TriangleUniforms));
    vkUnmapMemory(logicalDevice, ufmMem);
}

/**
* [RenderPassVk::SetupDescriptorPool description]
*/

void RenderPassVk::SetupDescriptorPool(VkDevice &logicalDevice)
{
    VkDescriptorPoolSize typeCounts[1];
    typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    typeCounts[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = typeCounts;
    descriptorPoolInfo.maxSets = 1;

    vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, nullptr, &descriptorPool);
}

/**
* [RenderPassVk::SetupDescriptorSetLayout description]
*/

void RenderPassVk::SetupDescriptorSetLayout(VkDevice &logicalDevice)
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = 1;
    descriptorLayout.pBindings = &layoutBinding;

    vkCreateDescriptorSetLayout(logicalDevice, &descriptorLayout, nullptr, &descriptorSetLayout);

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = 1;
    pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    vkCreatePipelineLayout(logicalDevice, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
}

/**
* [RenderPassVk::SetupDescriptorSet description]
*/

void RenderPassVk::SetupDescriptorSet(VkDevice &logicalDevice)
{
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    vkAllocateDescriptorSets(logicalDevice, &allocInfo, &descriptorSet);

    VkWriteDescriptorSet writeDescriptorSet = {};

    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo = &ufmDesc;
    writeDescriptorSet.dstBinding = 0;

    vkUpdateDescriptorSets(logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
}

/**
* [RenderPassVk::SetupPipelineState description]
*/

void RenderPassVk::SetupPipelineState(VkDevice &logicalDevice)
{
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};

    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};

    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterizationState = {};

    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};

    blendAttachmentState[0].colorWriteMask = 0xf;
    blendAttachmentState[0].blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};

    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = blendAttachmentState;

    VkPipelineViewportStateCreateInfo viewportState = {};

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vector<VkDynamicState> dynamicStateEnables;
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

    VkPipelineDynamicStateCreateInfo dynamicState = {};

    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables.data();
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};

    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = depthStencilState.back;

    VkPipelineMultisampleStateCreateInfo multisampleState = {};

    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.pSampleMask = nullptr;

    VkVertexInputBindingDescription vertexInputBinding = {};

    vertexInputBinding.binding = 0;
    vertexInputBinding.stride = 6 * sizeof(float);
    vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    array<VkVertexInputAttributeDescription, 2> vertexInputAttributs;

    vertexInputAttributs[0].binding = 0;
    vertexInputAttributs[0].location = 0;
    vertexInputAttributs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[0].offset = 0;

    vertexInputAttributs[1].binding = 0;
    vertexInputAttributs[1].location = 1;
    vertexInputAttributs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[1].offset = 3 * sizeof(float);

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};

    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
    vertexInputState.vertexAttributeDescriptionCount = 2;
    vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    shaderStages[0].sType           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage           = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module          = LoadShader(logicalDevice, "shaders/simple.vert.spv");
    shaderStages[0].pName           = "main";

    shaderStages[1].sType           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage           = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module          = LoadShader(logicalDevice, "shaders/simple.frag.spv");
    shaderStages[1].pName           = "main";

    pipelineCreateInfo.stageCount   = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages      = shaderStages.data();

    pipelineCreateInfo.pVertexInputState    = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState  = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState  = &rasterizationState;
    pipelineCreateInfo.pColorBlendState     = &colorBlendState;
    pipelineCreateInfo.pMultisampleState    = &multisampleState;
    pipelineCreateInfo.pViewportState       = &viewportState;
    pipelineCreateInfo.pDepthStencilState   = &depthStencilState;
    pipelineCreateInfo.renderPass           = renderPass;
    pipelineCreateInfo.pDynamicState        = &dynamicState;

    CHECK_RESULT(vkCreateGraphicsPipelines(logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline));
    vkDestroyShaderModule(logicalDevice, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(logicalDevice, shaderStages[1].module, nullptr);
}

/**
 * RenderPassVk::BuildCommandBuffers
 */

void RenderPassVk::BuildCommandBuffers(VkDevice &logicalDevice, VkCommandBuffer &cmdBuf)
{
    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    VkClearValue clearValues[2];
    clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = g_settings.winW;
    renderPassBeginInfo.renderArea.extent.height = g_settings.winH;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    for (uint32_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
        renderPassBeginInfo.framebuffer = frameBuffers[i];

        vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo);
        vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.height = (float)g_settings.winH;
        viewport.width = (float)g_settings.winW;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.extent.width = g_settings.winW;
        scissor.extent.height = g_settings.winH;
        scissor.offset.x = 0;
        scissor.offset.y = 0;

        vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &vertBuf, offsets);
        vkCmdBindIndexBuffer(drawCmdBuffers[i], idxBuf, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(drawCmdBuffers[i], 3, 1, 0, 0, 1);
        vkCmdEndRenderPass(drawCmdBuffers[i]);

        vkEndCommandBuffer(drawCmdBuffers[i]);
    }
}


/**
 * RenderPassVk::GetRenderPass - Get render pass info for this pass. Needed by the
 * main VK renderer to attach default frame buffer to output of this pass.
 */

VkRenderPass RenderPassVk::GetRenderPass()
{
    return renderPass;
}