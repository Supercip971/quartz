#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_structs.hpp>

namespace plt {

struct VertexDescription {
    size_t size;
    vk::VertexInputBindingDescription binding;
    std::vector<vk::VertexInputAttributeDescription> attributes;

    void dump() const {
        debug$(" - size: {}", size);
        debug$(" - binding:");
        debug$("  - binding: {}", (uint32_t)binding.binding);
        debug$("  - stride: {}", (uint32_t)binding.stride);
        debug$("  - input rate: {}", (uint32_t)binding.inputRate);
        debug$(" - attributes:");
        int i = 0;
        for (auto &attr : attributes) {
            debug$("  - attribute[{}]:", i);
            debug$("   - location: {}", (uint32_t)attr.location);
            debug$("   - binding: {}", (uint32_t)attr.binding);
            debug$("   - format: {}", (uint32_t)attr.format);
            debug$("   - offset: {}", (uint32_t)attr.offset);
            i++;
        }
    }
};

struct NVertex {
    glm::vec3 position;
    glm::vec3 color;

    NVertex() = default;
    NVertex(glm::vec3 position, glm::vec3 color) {
        this->position = position;
        this->color = color;
    }

    constexpr static vk::VertexInputBindingDescription binding() {

        return vk::VertexInputBindingDescription()
            .setBinding(0)
            .setStride(sizeof(NVertex))
            .setInputRate(vk::VertexInputRate::eVertex);
    }

    constexpr static uint32_t numAttributes = 2;
    constexpr static auto attributes() {
        return std::array{
            vk::VertexInputAttributeDescription()
                .setBinding(0)
                .setLocation(0)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(NVertex, position)),
            vk::VertexInputAttributeDescription()
                .setBinding(0)
                .setLocation(1)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(NVertex, color))};
    }

    static constexpr VertexDescription description() {
        std::vector<vk::VertexInputAttributeDescription> attrs;
        attrs.reserve(numAttributes);
        for (auto &attr : attributes()) {
            attrs.push_back(attr);
        }
        return VertexDescription{
            sizeof(NVertex),
            binding(),
            attrs,
        };
    }
};

template <typename T>
concept VertexType = requires(T v) {
                         { v.binding() } -> std::same_as<vk::VertexInputBindingDescription>;
                         { v.attributes() } -> std::same_as<std::array<vk::VertexInputAttributeDescription, T::numAttributes>>;
                         { v.description() } -> std::same_as<VertexDescription>;
                     };

static_assert(VertexType<NVertex>, "must be a vertex type");

} // namespace plt
