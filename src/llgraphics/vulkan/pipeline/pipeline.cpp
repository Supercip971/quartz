

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
namespace plt {

Result<> VkGfx::createGraphicPipeline() {

    debug$("creating graphic pipeline");

    std::vector<vk::DynamicState> dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor};

    auto dynamic_states_info = vk::PipelineDynamicStateCreateInfo()
                                   .setDynamicStates(dynamic_states);

    // describing how the vertex are in memory and how they are passed to the vertex shader

    auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo()
                                 .setVertexBindingDescriptions({})
                                 .setVertexAttributeDescriptions({});

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

    /* temporary */
    (void)colorBlending;
    (void)multisampling;
    (void)rasterizer;
    (void)viewport_state;
    (void)input_assembly;
    (void)vertex_input_info;
    (void)dynamic_states_info;

    vkTry$(this->LogicalDevice.createPipelineLayout(&pipeline_layout_info, nullptr, &this->pipelineLayout));

    this->deinit_funcs.push_back(
        [](VkGfx *self) {
            self->LogicalDevice.destroyPipelineLayout(self->pipelineLayout, nullptr);
        });

    return {};
}
} // namespace plt
