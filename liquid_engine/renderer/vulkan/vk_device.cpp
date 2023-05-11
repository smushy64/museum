/**
 * Description:  Vulkan Device Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 10, 2023
*/
#include "vk_device.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/collections.h"

#define DEVICE_HAS_GRAPHICS_BIT           (1 << 0)
#define DEVICE_HAS_PRESENT_BIT            (1 << 1)
#define DEVICE_HAS_COMPUTE_BIT            (1 << 2)
#define DEVICE_HAS_TRANSFER_BIT           (1 << 3)
#define DEVICE_HAS_SAMPLER_ANISOTROPY_BIT (1 << 4)
#define DEVICE_IS_DISCRETE_BIT            (1 << 5)

#define DEVICE_ALL_REQUIRED_FEATURES (\
    DEVICE_HAS_GRAPHICS_BIT           |\
    DEVICE_HAS_PRESENT_BIT            |\
    DEVICE_HAS_COMPUTE_BIT            |\
    DEVICE_HAS_TRANSFER_BIT           |\
    DEVICE_HAS_SAMPLER_ANISOTROPY_BIT |\
    DEVICE_IS_DISCRETE_BIT\
)

typedef u8 DevicePropertiesBitField;

struct VulkanDeviceRequirements {
    DevicePropertiesBitField properties;
    // list
    const char** device_extensions;
};

struct VulkanDeviceQueueFamilyInfo {
    i32 graphics_family_index;
    i32 present_family_index;
    i32 compute_family_index;
    i32 transfer_family_index;
};

internal b32 does_device_meet_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR     surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures*   features,
    const VulkanDeviceRequirements*   requirements,
    VulkanDeviceQueueFamilyInfo* out_queue_info,
    VulkanSwapchainSupportInfo*  out_swapchain_info
);

internal const char* to_string( VkPhysicalDeviceType type ) {
    #define MAX_DEVICE_TYPE 5
    local const char* strings[MAX_DEVICE_TYPE] = {
        "Unknown",
        "Integrated GPU",
        "Discrete GPU",
        "Virtual GPU",
        "CPU"
    };
    if( type >= MAX_DEVICE_TYPE ) {
        return strings[0];
    }
    return strings[type];
}

#define MAX_PHYSICAL_DEVICES 8
internal b32 select_physical_device( VulkanContext* context ) {
    u32 physical_device_count = 0;
    VK_ASSERT( vkEnumeratePhysicalDevices(
        context->instance,
        &physical_device_count,
        nullptr
    ) );
    if( physical_device_count == 0 ) {
        VK_LOG_ERROR(
            "There are no physical devices "
            "that support Vulkan on this machine!"
        );
        return false;
    }
    usize actual_count = physical_device_count > MAX_PHYSICAL_DEVICES ?
        MAX_PHYSICAL_DEVICES : physical_device_count;
    VkPhysicalDevice physical_devices[MAX_PHYSICAL_DEVICES];
    VK_ASSERT( vkEnumeratePhysicalDevices(
        context->instance,
        &physical_device_count,
        physical_devices
    ) );

    VulkanDeviceRequirements requirements = {};
    requirements.properties        = DEVICE_ALL_REQUIRED_FEATURES;
    requirements.device_extensions = list_reserve(const char*, 5);

    list_push(
        requirements.device_extensions,
        &VK_KHR_SWAPCHAIN_EXTENSION_NAME
    );

    VK_LOG_NOTE("Enumerating physical devices . . .");
    for( usize i = 0; i < actual_count; ++i ) {
        VkPhysicalDeviceProperties       properties;
        VkPhysicalDeviceFeatures         features;
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceProperties(
            physical_devices[i],
            &properties
        );
        vkGetPhysicalDeviceFeatures(
            physical_devices[i],
            &features
        );
        vkGetPhysicalDeviceMemoryProperties(
            physical_devices[i],
            &memory_properties
        );

        VulkanDeviceQueueFamilyInfo queue_info = {};
        if( does_device_meet_requirements(
            physical_devices[i],
            // TODO(alicia): temp!
            context->surfaces[0],
            &properties,
            &features,
            &requirements,
            &queue_info,
            &context->device.swapchain_support
        ) ) {
            VK_LOG_NOTE("Selected Device: %s", properties.deviceName);
            VK_LOG_NOTE("  Type:               %s",
                to_string(properties.deviceType)
            );
            VK_LOG_NOTE("  Driver Version:     %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion)
            );
            VK_LOG_NOTE("  Vulkan API Version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion)
            );

            context->device.physical_device = physical_devices[i];

            context->device.graphics_index = queue_info.graphics_family_index;
            context->device.present_index  = queue_info.present_family_index;
            context->device.transfer_index = queue_info.transfer_family_index;
            context->device.compute_index  = queue_info.compute_family_index;

            context->device.properties        = properties;
            context->device.features          = features;
            context->device.memory_properties = memory_properties;

            break;
        }

    }

    list_free( requirements.device_extensions );

    if( !context->device.physical_device ) {
        VK_LOG_ERROR("No suitable physical devices found!");
        return false;
    }

    return true;
}

internal b32 does_device_meet_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR     surface,
    const VkPhysicalDeviceProperties*       properties,
    const VkPhysicalDeviceFeatures*         features,
    const VulkanDeviceRequirements*         requirements,
    VulkanDeviceQueueFamilyInfo* out_queue_info,
    VulkanSwapchainSupportInfo*  out_swapchain_info
) {

    LOG_ASSERT( surface, "Surface is null!" );

    out_queue_info->compute_family_index  = -1;
    out_queue_info->present_family_index  = -1;
    out_queue_info->compute_family_index  = -1;
    out_queue_info->transfer_family_index = -1;

    u32 present_properties = 0;
    if( ARE_BITS_SET(
        requirements->properties,
        DEVICE_HAS_SAMPLER_ANISOTROPY_BIT
    ) ) {
        if( features->samplerAnisotropy ) {
            present_properties |= DEVICE_HAS_SAMPLER_ANISOTROPY_BIT;
        }
    }

    if( ARE_BITS_SET(
        requirements->properties,
        DEVICE_IS_DISCRETE_BIT
    ) ) {
        if(
            properties->deviceType !=
            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        ) {
            return false;
        }
    }
    present_properties |= DEVICE_IS_DISCRETE_BIT;

    #define MAX_QUEUE_FAMILY_COUNT 15
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queue_family_count,
        nullptr
    );
    SM_ASSERT( queue_family_count < MAX_QUEUE_FAMILY_COUNT );
    VkQueueFamilyProperties queue_family_properties[MAX_QUEUE_FAMILY_COUNT];
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queue_family_count,
        queue_family_properties
    );

    u32 min_transfer_score = 255;
    for( u32 i = 0; i < queue_family_count; ++i ) {
        u32 current_transfer_score = 0;

        if( ARE_BITS_SET(
            queue_family_properties[i].queueFlags,
            VK_QUEUE_GRAPHICS_BIT
        ) ) {
            out_queue_info->graphics_family_index = i;
            ++current_transfer_score;
            present_properties |= DEVICE_HAS_GRAPHICS_BIT;
        }

        if( ARE_BITS_SET(
            queue_family_properties[i].queueFlags,
            VK_QUEUE_COMPUTE_BIT
        ) ) {
            out_queue_info->compute_family_index = i;
            ++current_transfer_score;
            present_properties |= DEVICE_HAS_COMPUTE_BIT;
        }

        if( ARE_BITS_SET(
            queue_family_properties[i].queueFlags,
            VK_QUEUE_TRANSFER_BIT
        ) ) {
            if( current_transfer_score <= min_transfer_score ) {
                min_transfer_score = current_transfer_score;
                out_queue_info->transfer_family_index = i;
                present_properties |= DEVICE_HAS_TRANSFER_BIT;
            }
        }

        VkBool32 supports_present = VK_FALSE;
        VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(
            device, i,
            surface, &supports_present
        ));
        if( supports_present ) {
            present_properties |= DEVICE_HAS_PRESENT_BIT;
            out_queue_info->present_family_index = i;
        }
    }

    if( !ARE_BITS_SET( present_properties, requirements->properties ) ) {
        return false;
    }

    if(
        ARE_BITS_SET(present_properties, DEVICE_HAS_PRESENT_BIT) &&
        out_queue_info->present_family_index == -1
    ) {
        return false;
    }
    if(
        ARE_BITS_SET(present_properties, DEVICE_HAS_GRAPHICS_BIT) &&
        out_queue_info->graphics_family_index == -1
    ) {
        return false;
    }
    if(
        ARE_BITS_SET(present_properties, DEVICE_HAS_TRANSFER_BIT) &&
        out_queue_info->transfer_family_index == -1
    ) {
        return false;
    }
    if(
        ARE_BITS_SET(present_properties, DEVICE_HAS_COMPUTE_BIT) &&
        out_queue_info->compute_family_index == -1
    ) {
        return false;
    }

    vk_device_query_swapchain_support(
        device,
        surface,
        out_swapchain_info
    );

    if(
        out_swapchain_info->format_count < 1 ||
        out_swapchain_info->present_mode_count < 1
    ) {
        if( out_swapchain_info->formats ) {
            mem_free( out_swapchain_info->formats );
        }
        if( out_swapchain_info->present_modes ) {
            mem_free( out_swapchain_info->present_modes );
        }
        return false;
    }

    if( requirements->device_extensions ) {
        u32 available_extension_count = 0;
        VkExtensionProperties* available_extensions = nullptr;
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(
            device, nullptr,
            &available_extension_count,
            nullptr
        ));
        if( available_extension_count != 0 ) {
            // TODO(alicia): maybe this allocation is not necessary, who knows
            available_extensions = (VkExtensionProperties*)mem_alloc(
                available_extension_count * sizeof(VkExtensionProperties),
                MEMTYPE_RENDERER
            );
            VK_ASSERT(vkEnumerateDeviceExtensionProperties(
                device, nullptr,
                &available_extension_count,
                available_extensions
            ));

            u32 required_extension_count = list_count(
                requirements->device_extensions
            );
            for( u32 i = 0; i < required_extension_count; ++i ) {
                b32 found = false;
                for( u32 j = 0; j < available_extension_count; ++j ) {
                    if( str_cmp(
                        requirements->device_extensions[i],
                        available_extensions[j].extensionName
                    ) ) {
                        found = true;
                        break;
                    }
                }

                if( !found ) {
                    mem_free( available_extensions );
                    return false;
                }
            }

            mem_free( available_extensions );
        }
    }

    return true;
}

void vk_device_query_swapchain_support(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    VulkanSwapchainSupportInfo* out_swapchain_info
) {
    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, surface,
        &out_swapchain_info->capabilities
    ));
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, surface,
        &out_swapchain_info->format_count,
        nullptr
    ));
    if( !out_swapchain_info->format_count ) {
        if( !out_swapchain_info->formats ) {
            out_swapchain_info->formats = (VkSurfaceFormatKHR*)mem_alloc(
                out_swapchain_info->format_count *
                    sizeof(VkSurfaceFormatKHR),
                MEMTYPE_RENDERER
            );
        }
        VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, surface,
            &out_swapchain_info->format_count,
            out_swapchain_info->formats
        ));
    }
    VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface,
        &out_swapchain_info->present_mode_count,
        nullptr
    ));
    if( !out_swapchain_info->present_mode_count ) {
        if( !out_swapchain_info->present_modes ) {
            out_swapchain_info->present_modes = (VkPresentModeKHR*)mem_alloc(
                out_swapchain_info->present_mode_count *
                    sizeof(VkPresentModeKHR),
                MEMTYPE_RENDERER
            );
        }
        VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, surface,
            &out_swapchain_info->present_mode_count,
            out_swapchain_info->present_modes
        ));
    }
}

b32 vk_device_create( VulkanContext* context ) {
    VK_LOG_NOTE("Selecting physical device . . .");
    if( !select_physical_device( context ) ) {
        return false;
    }
    VK_LOG_NOTE("Creating logical device . . .");

    b32 present_shares_graphics_queue =
        context->device.graphics_index ==
        context->device.present_index;
    b32 transfer_shares_graphics_queue =
        context->device.graphics_index ==
        context->device.transfer_index;
    #define MAX_INDEX_COUNT 3

    i32 indices[MAX_INDEX_COUNT];
    u32 index_count = 0;
    indices[index_count++] = context->device.graphics_index;
    if( !present_shares_graphics_queue ) {
        indices[index_count++] = context->device.present_index;
    }
    if( !transfer_shares_graphics_queue ) {
        indices[index_count++] = context->device.transfer_index;
    }

    VkDeviceQueueCreateInfo queue_create_infos[MAX_INDEX_COUNT];
    #define MAX_QUEUE_COUNT 2
    f32 queue_priorities[MAX_QUEUE_COUNT] = {1.0f, 1.0f};
    for( u32 i = 0; i < index_count; ++i ) {
        queue_create_infos[i] = {};
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount       = 1;
        if( indices[i] == context->device.graphics_index ) {
            queue_create_infos[i].queueCount = MAX_QUEUE_COUNT;
        }
        queue_create_infos[i].pQueuePriorities = queue_priorities;
    }

    // TODO(alicia): temp
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount  = index_count;
    device_create_info.pQueueCreateInfos     = queue_create_infos;
    device_create_info.pEnabledFeatures      = &device_features;

    // TODO(alicia): temp
    local const char* device_extension_names[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    device_create_info.enabledExtensionCount = STATIC_ARRAY_COUNT(
        device_extension_names
    );
    device_create_info.ppEnabledExtensionNames = device_extension_names;

    VK_ASSERT(vkCreateDevice(
        context->device.physical_device,
        &device_create_info,
        context->allocator,
        &context->device.logical_device
    ));
    VK_LOG_NOTE("Logical device created.");

    for( u32 i = 0; i < VK_DEVICE_QUEUE_COUNT; ++i ) {
        vkGetDeviceQueue(
            context->device.logical_device,
            context->device.queue_indices[i],
            // TODO(alicia): temp
            0,
            &context->device.queues[i]
        );
        LOG_ASSERT(
            &context->device.queues[i],
            "Failed to get queue index %llu!", i
        );
    }
    VK_LOG_NOTE("Queues obtained.");

    return true;
}

void vk_device_destroy( VulkanContext* context ) {
    vkDestroyDevice(
        context->device.logical_device,
        context->allocator
    );
    VK_LOG_NOTE("Logical device destroyed.");
    if( context->device.swapchain_support.formats ) {
        mem_free( context->device.swapchain_support.formats );
    }
    if( context->device.swapchain_support.present_modes ) {
        mem_free( context->device.swapchain_support.present_modes );
    }
    context->device = {};
}
