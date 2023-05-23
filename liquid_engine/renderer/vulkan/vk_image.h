#if !defined(VK_IMAGE_HPP)
#define VK_IMAGE_HPP
/**
 * Description:  Vulkan Image utilities
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "vk_defines.h"

void vk_image_create(
    VulkanContext*        context,
    VkImageType           type,
    u32                   width,
    u32                   height,
    VkFormat              format,
    VkImageTiling         tiling,
    VkImageUsageFlags     usage,
    VkMemoryPropertyFlags memory_flags,
    b32                   create_view,
    VkImageAspectFlags    aspect_flags,
    VulkanImage*          out_image
);

void vk_image_view_create(
    VulkanContext*     context,
    VkFormat           format,
    VulkanImage*       image,
    VkImageAspectFlags aspect_flags
);

void vk_image_destroy(
    VulkanContext* context,
    VulkanImage*   image
);

#endif // header guard