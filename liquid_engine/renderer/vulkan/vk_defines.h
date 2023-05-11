#if !defined(VK_DEFINES_HPP)
#define VK_DEFINES_HPP
/**
 * Description:  Vulkan specific defines
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "defines.h"
#include <vulkan/vulkan.h>
#include "core/logging.h"

#if defined(LD_LOGGING)
    #define VK_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[VK NOTE ] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[VK INFO ] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[VK DEBUG] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[VK WARN ] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[VK ERROR] " format,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[VK NOTE  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[VK INFO  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[VK DEBUG | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define VK_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[VK WARN  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[VK ERROR | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
#else
    #define VK_LOG_NOTE( format, ... )
    #define VK_LOG_INFO( format, ... )
    #define VK_LOG_DEBUG( format, ... )
    #define VK_LOG_WARN( format, ... )
    #define VK_LOG_ERROR( format, ... )
    #define VK_LOG_NOTE_TRACE( format, ... )
    #define VK_LOG_INFO_TRACE( format, ... )
    #define VK_LOG_DEBUG_TRACE( format, ... )
    #define VK_LOG_WARN_TRACE( format, ... )
    #define VK_LOG_ERROR_TRACE( format, ... )
#endif

#define VK_KHR_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"

#define VK_ASSERT(expr) do {\
        VkResult result = expr;\
        LOG_ASSERT(\
            result == VK_SUCCESS,\
            "Vulkan Fatal Error: 0x%X",\
            result\
        );\
    } while(0)

struct VulkanSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    u32 present_mode_count;
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR*   present_modes;
};

#define VK_DEVICE_QUEUE_COUNT 4
struct VulkanDevice {
    VkPhysicalDeviceProperties       properties;
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPhysicalDeviceFeatures         features;
    VulkanSwapchainSupportInfo swapchain_support;
    union {
        i32 queue_indices[VK_DEVICE_QUEUE_COUNT];
        struct {
            i32 graphics_index;
            i32 present_index;
            i32 transfer_index;
            i32 compute_index;
        };
    };
    union {
        VkQueue queues[VK_DEVICE_QUEUE_COUNT];
        struct {
            VkQueue graphics_queue;
            VkQueue present_queue;
            VkQueue transfer_queue;
            VkQueue compute_queue;
        };
    };
    VkPhysicalDevice physical_device;
    VkDevice         logical_device;
};

struct VulkanContext {
    VulkanDevice device;

    VkSurfaceKHR           surfaces[MAX_SURFACE_COUNT];
    VkInstance             instance;
    VkAllocationCallbacks* allocator;

#if defined(DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
};

#define VK_MAX_EXTENSIONS 10

#endif // header guard