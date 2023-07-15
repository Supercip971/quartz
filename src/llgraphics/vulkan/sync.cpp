
#include "llgraphics/vulkan/vk_gfx.hpp"
namespace plt {

Result<> VkGfx::createSyncObjects() {

    debug$("creating sync objects...");

    auto semaphoreInfo = vk::SemaphoreCreateInfo();
    auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

    vkTry$(this->LogicalDevice.createSemaphore(&semaphoreInfo, nullptr, &this->imageAvailableSemaphore));
    vkTry$(this->LogicalDevice.createSemaphore(&semaphoreInfo, nullptr, &this->renderFinishedSemaphore));
    vkTry$(this->LogicalDevice.createFence(&fenceInfo, nullptr, &this->inFlightFence));

    this->deinit_funcs.push_back([](VkGfx *self) {
        debug$("destroying sync objects...");
        self->LogicalDevice.destroySemaphore(self->imageAvailableSemaphore);
        self->LogicalDevice.destroySemaphore(self->renderFinishedSemaphore);
        self->LogicalDevice.destroyFence(self->inFlightFence);
    });
    return {};
}

} // namespace plt
