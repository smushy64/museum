/**
 * Description:  Vulkan Swapchain
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 11, 2023
*/
#include "vk_swapchain.h"
#include "vk_device.h"
#include "vk_image.h"
#include "core/memory.h"
#include "core/collections.h"
#include "core/logging.h"
#include "core/math/functions.h"

#define MAX_FRAMES_IN_FLIGHT 2
global b32 SURFACE_FORMAT_CHOSEN = false;
global VkSurfaceFormatKHR SURFACE_FORMAT;
global b32 PRESENT_MODE_CHOSEN = false;
global VkPresentModeKHR PRESENT_MODE = VK_PRESENT_MODE_FIFO_KHR;

internal void impl_swapchain_create(
    VulkanContext* context,
    u32 width, u32 height,
    VulkanSwapchain* out_swapchain,
    VkSwapchainKHR opt_old_swapchain
) {
    VkSwapchainCreateInfoKHR swapchain_info = {};

    VkExtent2D extents = { width, height };
    out_swapchain->max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;

    if( !SURFACE_FORMAT_CHOSEN ) {
        b32 found = false;
        for(
            u32 i = 0;
            i < context->device.swapchain_support.format_count;
            ++i
        ) {
            VkSurfaceFormatKHR format =
                context->device.swapchain_support.formats[i];
            if(
                format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            ) {
                SURFACE_FORMAT_CHOSEN = true;
                SURFACE_FORMAT = format;
                found = true;
                break;
            }
        }

        // just use the first format available if prefered format is not found
        if( !found ) {
            SURFACE_FORMAT = context->device.swapchain_support.formats[0];
            SURFACE_FORMAT_CHOSEN = true;
        }
    }

    out_swapchain->image_format    = SURFACE_FORMAT;
    swapchain_info.imageFormat     = out_swapchain->image_format.format;
    swapchain_info.imageColorSpace = out_swapchain->image_format.colorSpace;

    VkPresentModeKHR present_mode = PRESENT_MODE;
    if( !PRESENT_MODE_CHOSEN ) {
        for(
            u32 i = 0;
            i < context->device.swapchain_support.present_mode_count;
            ++i
        ) {
            VkPresentModeKHR current_present_mode =
                context->device.swapchain_support.present_modes[i];
            if( current_present_mode == VK_PRESENT_MODE_MAILBOX_KHR ) {
                present_mode = current_present_mode;
                break;
            }
        }

        PRESENT_MODE        = present_mode;
        PRESENT_MODE_CHOSEN = true;
    }
    swapchain_info.presentMode = present_mode;

    vk_device_query_swapchain_support(
        context->device.physical_device,
        context->surface.surface,
        &context->device.swapchain_support
    );

    if(
        context->device.swapchain_support.capabilities
            .currentExtent.width != U32::MAX
    ) {
        extents = context->device.swapchain_support
            .capabilities.currentExtent;
    }

    VkExtent2D min_extents = context->device.swapchain_support
        .capabilities.minImageExtent;
    VkExtent2D max_extents = context->device.swapchain_support
        .capabilities.maxImageExtent;
    extents.width = clamp(
        extents.width,
        min_extents.width,
        max_extents.width
    );
    extents.height = clamp(
        extents.height,
        min_extents.height,
        max_extents.height
    );

    u32 min_image_count = context->device.swapchain_support
        .capabilities.minImageCount + 1;
    
    min_image_count = min(
        min_image_count,
        context->device.swapchain_support
            .capabilities.maxImageCount
    );

    swapchain_info.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = context->surface.surface;

    swapchain_info.minImageCount    = min_image_count;
    swapchain_info.imageExtent      = extents;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if(
        context->device.graphics_index !=
        context->device.present_index
    ) {

        u32 QUEUE_FAMILY_INDICES[] = {
            (u32)context->device.graphics_index,
            (u32)context->device.present_index,
        };

        swapchain_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount =
            STATIC_ARRAY_COUNT(QUEUE_FAMILY_INDICES);
        swapchain_info.pQueueFamilyIndices   = QUEUE_FAMILY_INDICES;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    // IMPORTANT(alicia): this needs to be handled differently on other
    // platforms
    swapchain_info.preTransform = context->device
        .swapchain_support
        .capabilities
        .currentTransform;

    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.clipped        = VK_TRUE;
    swapchain_info.oldSwapchain   = opt_old_swapchain;

    VK_ASSERT(vkCreateSwapchainKHR(
        context->device.logical_device,
        &swapchain_info,
        context->allocator,
        &out_swapchain->swapchain
    ));

    context->current_frame = 0;

    b32 fill_image_list = false;
    u32 image_count = 0;
    VK_ASSERT(vkGetSwapchainImagesKHR(
        context->device.logical_device,
        out_swapchain->swapchain,
        &image_count,
        nullptr
    ));
    if( !out_swapchain->images ) {
        out_swapchain->images = list_reserve( VkImage, image_count );
        list_set_count( out_swapchain->images, image_count );
        fill_image_list = true;

    } else if( image_count != list_count(out_swapchain->images) ) {
        out_swapchain->images = list_realloc(
            out_swapchain->images,
            image_count
        );
        list_set_count( out_swapchain->images, image_count );
        fill_image_list = true;
    }

    LOG_ASSERT(
        out_swapchain->images,
        "Images list is null!"
    );

    if( fill_image_list ) {
        VK_ASSERT(vkGetSwapchainImagesKHR(
            context->device.logical_device,
            out_swapchain->swapchain,
            &image_count,
            out_swapchain->images
        ));
    }

    if( !out_swapchain->image_views ) {
        out_swapchain->image_views = list_reserve(
            VkImageView,
            image_count
        );
        list_set_count(
            out_swapchain->image_views,
            image_count
        );
    }
    
    VkImageViewCreateInfo image_view_create_info = {};
    image_view_create_info.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format   = out_swapchain->image_format.format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.layerCount = 1;

    for( u32 i = 0; i < image_count; ++i ) {
        image_view_create_info.image = out_swapchain->images[i];
        VK_ASSERT(vkCreateImageView(
            context->device.logical_device,
            &image_view_create_info,
            context->allocator,
            &out_swapchain->image_views[i]
        ));
    }

    if( !vk_device_detect_depth_format( &context->device ) ) {
        LOG_FATAL( "Failed to find a supported depth format!" );
        SM_PANIC();
    }

    vk_image_create(
        context,
        VK_IMAGE_TYPE_2D,
        width,
        height,
        context->device.depth_buffer_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &out_swapchain->depth_attachment
    );

}

internal void impl_swapchain_destroy(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    b32 free_all
) {
    vk_image_destroy(
        context,
        &swapchain->depth_attachment
    );

    u32 view_count = list_count( swapchain->image_views );
    for( u32 i = 0; i < view_count; ++i ) {
        vkDestroyImageView(
            context->device.logical_device,
            swapchain->image_views[i],
            context->allocator
        );
    }

    vkDestroySwapchainKHR(
        context->device.logical_device,
        swapchain->swapchain,
        context->allocator
    );

    if( free_all ) {
        list_free( swapchain->image_views );
        list_free( swapchain->images );
    }
}


void vk_swapchain_create(
    VulkanContext*   context,
    u32 width,   u32 height,
    VulkanSwapchain* out_swapchain
) {
    impl_swapchain_create(
        context,
        width, height,
        out_swapchain,
        nullptr
    );

    VK_LOG_NOTE("Swapchain created successfully.");
}
void vk_swapchain_recreate(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    u32 width,   u32 height
) {
    impl_swapchain_destroy(
        context,
        swapchain,
        false
    );
    vk_swapchain_create(
        context,
        width, height,
        swapchain
    );
}
void vk_swapchain_destroy(
    VulkanContext*   context,
    VulkanSwapchain* swapchain
) {
    impl_swapchain_destroy(
        context,
        swapchain,
        true
    );
}
b32 vk_swapchain_next_image_index(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    u64              timeout_ms,
    VkSemaphore      image_available,
    VkFence          fence,
    u32*             out_image_index
) {
    VkResult result = vkAcquireNextImageKHR(
        context->device.logical_device,
        swapchain->swapchain,
        timeout_ms,
        image_available,
        fence,
        out_image_index
    );
    if( result == VK_ERROR_OUT_OF_DATE_KHR ) {
        vk_swapchain_recreate(
            context,
            swapchain,
            context->surface.width,
            context->surface.height
        );
        return false;
    } else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ) {
        VK_LOG_ERROR("Failed to obtain swapchain image!");
        return false;
    }
    return true;
}
void vk_swapchain_present(
    VulkanContext*   context,
    VulkanSwapchain* swapchain,
    VkQueue          graphics_queue,
    VkQueue          present_queue,
    VkSemaphore      render_complete,
    u32              present_image_index
) {
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &render_complete;
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &swapchain->swapchain;
    present_info.pImageIndices      = &present_image_index;
    VkResult present_result = vkQueuePresentKHR(
        present_queue,
        &present_info
    );

    if(
        present_result == VK_ERROR_OUT_OF_DATE_KHR ||
        present_result == VK_SUBOPTIMAL_KHR
    ) {
        vk_swapchain_recreate(
            context,
            swapchain,
            context->surface.width,
            context->surface.height
        );
    }
    LOG_ASSERT(
        present_result != VK_SUCCESS,
        "Queue Present failed! result: 0x%X",
        (u32)present_result
    );

    // TODO(alicia): 
    SM_UNUSED(graphics_queue);
}
