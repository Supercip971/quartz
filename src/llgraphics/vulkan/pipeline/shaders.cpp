
#include "llgraphics/vulkan/pipeline/shaders.hpp"
#include <filesystem>

#include "llgraphics/vulkan/vk_gfx.hpp"

namespace plt {

Result<> VkGfx::createShaderPipeline() {

    debug$("creating shader pipeline");

    this->vertShader = try$(Shader::fromFile(this->LogicalDevice, "./.cutekit/build/bundles/shaders/main-vert.o", ShaderKind::Vertex));
    this->fragShader = try$(Shader::fromFile(this->LogicalDevice, "./.cutekit/build/bundles/shaders/main-frag.o", ShaderKind::Fragment));

    this->shaderPipeline = ShaderPipeline();
    this->shaderPipeline.addShader(this->vertShader);
    this->shaderPipeline.addShader(this->fragShader);

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        gfx->vertShader.deinit(gfx->LogicalDevice);
        gfx->fragShader.deinit(gfx->LogicalDevice);
    });
    return {};
}
} // namespace plt
