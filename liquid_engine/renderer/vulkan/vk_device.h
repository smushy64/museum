#if !defined(VK_DEVICE_HPP)
#define VK_DEVICE_HPP
/**
 * Description:  Vulkan Device
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 10, 2023
*/
#include "defines.h"
#include "vk_defines.h"

b32 vk_device_create( VulkanContext* context );
void vk_device_destroy( VulkanContext* context );

void vk_device_query_swapchain_support(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    VulkanSwapchainSupportInfo* out_swapchain_info
);

b32 vk_device_detect_depth_format( VulkanDevice* device );

#endif // header guard