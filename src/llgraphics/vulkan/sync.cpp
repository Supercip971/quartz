
#include "llgraphics/vulkan/vk_gfx.hpp"
namespace plt {

Result<> VkGfx::createSyncObjects() {

    this->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    this->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    this->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    auto semaphoreInfo = vk::SemaphoreCreateInfo();
    auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        debug$("creating sync objects (frame: {})...", i);

        vkTry$(this->LogicalDevice.createSemaphore(&semaphoreInfo, nullptr, &this->imageAvailableSemaphores[i]));
        vkTry$(this->LogicalDevice.createSemaphore(&semaphoreInfo, nullptr, &this->renderFinishedSemaphores[i]));
        vkTry$(this->LogicalDevice.createFence(&fenceInfo, nullptr, &this->inFlightFences[i]));

        this->deinit_funcs.push_back([i](VkGfx *self) {
            debug$("destroying sync objects (frame: {})...", i);
            self->LogicalDevice.destroySemaphore(self->imageAvailableSemaphores[i]);
            self->LogicalDevice.destroySemaphore(self->renderFinishedSemaphores[i]);
            self->LogicalDevice.destroyFence(self->inFlightFences[i]);
        });
    }
    return {};
}

} // namespace plt
