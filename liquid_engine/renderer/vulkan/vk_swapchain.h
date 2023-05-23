#if !defined(VK_SWAPCHAIN_HPP)
#define VK_SWAPCHAIN_HPP
/**
 * Description:  Vulkan Swapchain
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 11, 2023
*/
#include "vk_defines.h"

void vk_swapchain_create(
    VulkanContext*   context,
    u32 width,   u32 height,
    VulkanSwapchain* out_swapchain
);
void vk_swapchain_recreate(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    u32 width,   u32 height
);
void vk_swapchain_destroy(
    VulkanContext*   context,
    VulkanSwapchain* swapchain
);
b32 vk_swapchain_next_image_index(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    u64              timeout_ms,
    VkSemaphore      image_available,
    VkFence          fence,
    u32*             out_image_index
);
void vk_swapchain_present(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    VkQueue          graphics_queue,
    VkQueue          present_queue,
    VkSemaphore      render_complete,
    u32              present_image_index
);

#endif // header guard