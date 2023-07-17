

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
namespace plt {

Result<> VkGfx::createGraphicPipeline(const VertexDescription &description) {

    debug$("creating graphic pipeline layout...");

	info$("with vertex description:");
	description.dump();
    std::vector<vk::DynamicState> dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor};

    auto dynamic_states_info = vk::PipelineDynamicStateCreateInfo()
                                   .setDynamicStates(dynamic_states);

    // describing how the vertex are in memory and how they are passed to the vertex shader

	auto bindingDesc = description.binding;
	auto attributeDesc = description.attributes;

    auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo()
                                 .setVertexBindingDescriptions(bindingDesc)
                                 .setVertexAttributeDescriptions(attributeDesc);

    // describing what topology the vertices are in
    auto input_assembly = vk::PipelineInputAssemblyStateCreateInfo()
                              .setTopology(vk::PrimitiveTopology::eTriangleList)
                              .setPrimitiveRestartEnable(false);

    // describing the viewport

    auto viewport = vk::Viewport()
                        .setX(0.0f)
                        .setY(0.0f)
                        .setWidth((float)this->swapchainExtent.width)
                        .setHeight((float)this->swapchainExtent.height)
                        .setMinDepth(0.0f)
                        .setMaxDepth(1.0f);

    auto scissor = vk::Rect2D()
                       .setOffset({0, 0})
                       .setExtent(this->swapchainExtent);

    auto viewport_state = vk::PipelineViewportStateCreateInfo()
                              .setViewportCount(1)
                              .setPViewports(&viewport)
                              .setScissorCount(1)
                              .setPScissors(&scissor);

    // describing the rasterizer

    auto rasterizer = vk::PipelineRasterizationStateCreateInfo()
                          .setDepthClampEnable(false)
                          .setRasterizerDiscardEnable(false)
                          .setPolygonMode(vk::PolygonMode::eFill)
                          .setLineWidth(1.0f)
                          .setCullMode(vk::CullModeFlagBits::eBack)
                          .setFrontFace(vk::FrontFace::eClockwise)
                          .setDepthBiasEnable(false);

    // describing multisampling

    auto multisampling = vk::PipelineMultisampleStateCreateInfo()
                             .setSampleShadingEnable(false)
                             .setRasterizationSamples(vk::SampleCountFlagBits::e1)
                             .setMinSampleShading(1.0f)
                             .setPSampleMask(nullptr)
                             .setAlphaToCoverageEnable(false)
                             .setAlphaToOneEnable(false);

    // describing color blending with the framebuffer

    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
                                    .setColorWriteMask(
                                        vk::ColorComponentFlagBits::eR |
                                        vk::ColorComponentFlagBits::eG |
                                        vk::ColorComponentFlagBits::eB |
                                        vk::ColorComponentFlagBits::eA)
                                    .setBlendEnable(false)
                                    .setSrcColorBlendFactor(vk::BlendFactor::eOne)
                                    .setDstColorBlendFactor(vk::BlendFactor::eZero)
                                    .setColorBlendOp(vk::BlendOp::eAdd)
                                    .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                                    .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                                    .setAlphaBlendOp(vk::BlendOp::eAdd);

    auto colorBlending = vk::PipelineColorBlendStateCreateInfo()
                             .setLogicOpEnable(false)
                             .setLogicOp(vk::LogicOp::eCopy)
                             .setAttachmentCount(1)
                             .setPAttachments(&colorBlendAttachment)
                             .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

    // describing the pipeline layout

    auto pipeline_layout_info = vk::PipelineLayoutCreateInfo()
                                    .setSetLayoutCount(0)
                                    .setPSetLayouts(nullptr)
                                    .setPushConstantRangeCount(0)
                                    .setPPushConstantRanges(nullptr);
    vkTry$(this->LogicalDevice.createPipelineLayout(&pipeline_layout_info, nullptr, &this->pipelineLayout));

    this->deinit_funcs.push_back(
        [](VkGfx *self) {
            debug$("destroying graphic pipeline layout");
            self->LogicalDevice.destroyPipelineLayout(self->pipelineLayout, nullptr);
        });

    // creating the graphic pipeline

    debug$("creating graphic pipeline...");

    auto shaderPipelineInfo = this->shaderPipeline.stageInfos();

    auto pipeline_info = vk::GraphicsPipelineCreateInfo()
                             .setStages(shaderPipelineInfo)
                             .setPVertexInputState(&vertex_input_info)
                             .setPInputAssemblyState(&input_assembly)
                             .setPViewportState(&viewport_state)
                             .setPRasterizationState(&rasterizer)
                             .setPMultisampleState(&multisampling)
                             .setPDepthStencilState(nullptr)
                             .setPColorBlendState(&colorBlending)
                             .setPDynamicState(&dynamic_states_info)
                             .setLayout(this->pipelineLayout)
                             .setRenderPass(this->renderPass)
                             .setSubpass(0)
                             .setBasePipelineHandle(nullptr)
                             .setBasePipelineIndex(-1);

    vkTry$(this->LogicalDevice.createGraphicsPipelines(nullptr, 1, &pipeline_info, nullptr, &this->graphicPipeline));

    this->deinit_funcs.push_back(
        [](VkGfx *self) {
            debug$("destroying graphic pipeline");
            self->LogicalDevice.destroyPipeline(self->graphicPipeline, nullptr);
        });

    return {};
}
} // namespace plt
