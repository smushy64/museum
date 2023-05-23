/**
 * Description:  Vulkan Image Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "vk_image.h"
#include "vk_device.h"
#include "core/memory.h"
#include "core/logging.h"

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
) {
    out_image->width  = width;
    out_image->height = height;

    VkImageCreateInfo create_info = {};
    create_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType     = type;
    create_info.extent.width  = width; 
    create_info.extent.height = height;
    // TODO(alicia): configurable depth
    create_info.extent.depth = 1;
    // TODO(alicia): configurable mipmaps
    create_info.mipLevels = 4;
    // TODO(alicia): configurable layers
    create_info.arrayLayers = 1;

    create_info.format        = format;
    create_info.tiling        = tiling;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage         = usage;
    // TODO(alicia): configurable sample count
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    // TODO(alicia): configurable sharing mode
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_ASSERT(vkCreateImage(
        context->device.logical_device,
        &create_info,
        context->allocator,
        &out_image->image
    ));

    VkMemoryRequirements memory_requirements = {};
    vkGetImageMemoryRequirements(
        context->device.logical_device,
        out_image->image,
        &memory_requirements
    );

    i32 memory_type = find_memory_index(
        context,
        memory_requirements.memoryTypeBits,
        memory_flags
    );
    if( memory_type == -1 ) {
        LOG_ERROR("Required memory type not found. Image is not valid!");
    }

    VkMemoryAllocateInfo malloc_info = {};
    malloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    malloc_info.allocationSize  = memory_requirements.size;
    malloc_info.memoryTypeIndex = memory_type;

    VK_ASSERT(vkAllocateMemory(
        context->device.logical_device,
        &malloc_info,
        context->allocator,
        &out_image->memory
    ));

    // TODO(alicia): configurable memory offset
    VK_ASSERT(vkBindImageMemory(
        context->device.logical_device,
        out_image->image,
        out_image->memory,
        0
    ));

    if( create_view ) {
        vk_image_view_create(
            context,
            format,
            out_image,
            aspect_flags
        );
    }

}

void vk_image_view_create(
    VulkanContext*     context,
    VkFormat           format,
    VulkanImage*       image,
    VkImageAspectFlags aspect_flags
) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image    = image->image;
    create_info.format   = format;
    create_info.subresourceRange.aspectMask = aspect_flags;

    // TODO(alicia): configurable
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.subresourceRange.baseMipLevel   = 0;
    create_info.subresourceRange.levelCount     = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount     = 1;

    VK_ASSERT(vkCreateImageView(
        context->device.logical_device,
        &create_info,
        context->allocator,
        &image->view
    ));
}

void vk_image_destroy(
    VulkanContext* context,
    VulkanImage*   image
) {
    if( image->view ) {
        vkDestroyImageView(
            context->device.logical_device,
            image->view,
            context->allocator
        );
    }
    if( image->memory ) {
        vkFreeMemory(
            context->device.logical_device,
            image->memory,
            context->allocator
        );
    }
    if( image->image ) {
        vkDestroyImage(
            context->device.logical_device,
            image->image,
            context->allocator
        );
    }
    *image = {};
}
