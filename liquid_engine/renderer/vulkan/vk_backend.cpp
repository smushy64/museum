/**
 * Description:  Vulkan Renderer Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "vk_defines.h"
#include "vk_backend.h"
#include "vk_device.h"
#include "platform/platform.h"
#include "core/collections.h"
#include "core/string.h"
#include "core/memory.h"

/// Extensions that are always required
global const char* REQUIRED_EXTENSION_NAMES[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(DEBUG)
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

#if defined(DEBUG)
/// Validation layers
global const char* REQUIRED_VALIDATION_LAYERS[] = {
    VK_KHR_VALIDATION_LAYER_NAME
};
#endif

global VulkanContext CONTEXT = {};

internal VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
);

b32 vk_init(
    struct RendererBackend* backend,
    const char* app_name,
    struct PlatformState* platform
) {

    // TODO(alicia): 
    CONTEXT.allocator = nullptr;

    VK_LOG_NOTE(
        "Initializing %s . . .",
        to_string(BACKEND_VULKAN)
    );

    VkApplicationInfo app_info = {};
    app_info.sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_API_VERSION(
        0, VULKAN_VERSION_MAJOR,
        VULKAN_VERSION_MINOR, 0
    );
    app_info.pApplicationName   = app_name;
    app_info.applicationVersion = LD_MAKE_VERSION(
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );
    app_info.pEngineName   = LIQUID_ENGINE_VERSION;
    app_info.engineVersion = LD_MAKE_VERSION(
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    VkInstanceCreateInfo instance_info = {};
    instance_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    
    #define MIN_EXTENSION_NAME_COUNT 5

    usize extension_count = 0;
    const char* extension_names[VK_MAX_EXTENSIONS] = {};
    usize required_extensions_count =
        STATIC_ARRAY_COUNT(REQUIRED_EXTENSION_NAMES);
    LOG_ASSERT(
        required_extensions_count < VK_MAX_EXTENSIONS,
        "VK_MAX_EXTENSIONS is too low!"
    );
    for( usize i = 0; i < required_extensions_count; ++i ) {
        extension_names[extension_count++] =
            REQUIRED_EXTENSION_NAMES[i];
    }

    usize remaining_platform_extensions = platform_get_vulkan_extension_names(
        VK_MAX_EXTENSIONS,
        &extension_count,
        extension_names
    );
    LOG_ASSERT(
        remaining_platform_extensions == 0,
        "VK_MAX_EXTENSIONS is too low! "
        "Remaining platform required extension count: %llu",
        remaining_platform_extensions
    );

#if defined(DEBUG)
    VK_LOG_NOTE("Required extensions:");
    for(
        usize i = 0;
        i < extension_count;
        ++i
    ) {
        VK_LOG_NOTE("    %llu: %s", i, extension_names[i]);
    }
#endif

    instance_info.enabledExtensionCount   = extension_count;
    instance_info.ppEnabledExtensionNames = extension_names;

    u32 required_layer_count = 0;
    const char** required_layer_names = nullptr;

#if defined(DEBUG)
    VK_LOG_DEBUG("Enabling validation layers . . .");

    required_layer_count = STATIC_ARRAY_COUNT(REQUIRED_VALIDATION_LAYERS);
    required_layer_names = REQUIRED_VALIDATION_LAYERS;

    u32 available_layer_count = 0;
    VK_ASSERT(vkEnumerateInstanceLayerProperties(
        &available_layer_count,
        nullptr
    ));
    VkLayerProperties* available_layers = list_reserve(
        VkLayerProperties,
        available_layer_count
    );
    list_set_count(
        available_layers,
        available_layer_count
    );
    VK_ASSERT(vkEnumerateInstanceLayerProperties(
        &available_layer_count,
        available_layers
    ));

    VK_LOG_NOTE(
        "%u Validation Layers available, searching for required layers . . .",
        available_layer_count
    );

    for( u32 i = 0; i < required_layer_count; ++i ) {
        b32 found = false;
        for( u32 j = 0; j < available_layer_count; ++j ) {
            if( str_cmp(
                REQUIRED_VALIDATION_LAYERS[i],
                available_layers[j].layerName
            ) ) {
                found = true;
                break;
            }
        }

        if( found ) {
            VK_LOG_NOTE(
                "    Required Validation Layer \"%s\" is available.",
                REQUIRED_VALIDATION_LAYERS[i]
            );
        } else {
            VK_LOG_ERROR(
                "    Required Validation Layer \"%s\" is not available!",
                REQUIRED_VALIDATION_LAYERS[i]
            );
            return false;
        }
    }

    VK_LOG_NOTE("All required Validation Layers are available.");

    list_free( available_layers );
#endif

    instance_info.enabledLayerCount   = required_layer_count;
    instance_info.ppEnabledLayerNames = required_layer_names;
    
    VK_ASSERT(vkCreateInstance(
        &instance_info,
        CONTEXT.allocator,
        &CONTEXT.instance
    ));

#if defined(DEBUG)
    VK_LOG_DEBUG("Initializing Vulkan Debugger . . .");

    u32 vk_log_level = 0;
    LogLevel current_log_level = query_log_level();
    if( ARE_BITS_SET( current_log_level, LOG_LEVEL_VERBOSE ) ) {
        vk_log_level |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    }
    if( ARE_BITS_SET( current_log_level, LOG_LEVEL_INFO ) ) {
        vk_log_level |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    }
    if( ARE_BITS_SET( current_log_level, LOG_LEVEL_WARN ) ) {
        vk_log_level |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    }
    if( ARE_BITS_SET( current_log_level, LOG_LEVEL_ERROR ) ) {
        vk_log_level |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    }

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = vk_log_level;
    debug_create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        CONTEXT.instance,
        "vkCreateDebugUtilsMessengerEXT"
    );

    LOG_ASSERT(
        vkCreateDebugUtilsMessengerEXT,
        "Failed to load debug utils messenger function!"
    );
    VK_ASSERT(vkCreateDebugUtilsMessengerEXT(
        CONTEXT.instance,
        &debug_create_info,
        CONTEXT.allocator,
        &CONTEXT.debug_messenger
    ));

    VK_LOG_DEBUG("Vulkan Debugger Initialized.");
#endif

    if( !platform_create_vulkan_surfaces(
        platform,
        &CONTEXT
    ) ) {
        return false;
    }

    if( !vk_device_create( &CONTEXT ) ) {
        return false;
    }

    SM_UNUSED(backend);
    VK_LOG_NOTE( "Vulkan backend initialized successfully." );
    return true;
}
void vk_on_resize(
    struct RendererBackend* backend,
    i32 width, i32 height
) {
    SM_UNUSED(backend);
    SM_UNUSED(width);
    SM_UNUSED(height);
}
b32 vk_begin_frame(
    struct RendererBackend* backend,
    f32 delta_time
) {
    SM_UNUSED(backend);
    SM_UNUSED(delta_time);
    return true;
}
b32 vk_end_frame(
    struct RendererBackend* backend,
    f32 delta_time
) {
    SM_UNUSED(backend);
    SM_UNUSED(delta_time);
    return true;
}

void vk_shutdown(
    struct RendererBackend* backend
) {

    for( usize i = 0; i < MAX_SURFACE_COUNT; ++i ) {
        if( CONTEXT.surfaces[i] ) {
            vkDestroySurfaceKHR(
                CONTEXT.instance,
                CONTEXT.surfaces[i],
                CONTEXT.allocator
            );
        }
    }

    vk_device_destroy( &CONTEXT );

#if defined(DEBUG)
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
    (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        CONTEXT.instance,
        "vkDestroyDebugUtilsMessengerEXT"
    );
    LOG_ASSERT(
        vkDestroyDebugUtilsMessengerEXT,
        "Failed to load debug utils destroy function!"
    );
    vkDestroyDebugUtilsMessengerEXT(
        CONTEXT.instance,
        CONTEXT.debug_messenger,
        CONTEXT.allocator
    );
#endif

    vkDestroyInstance(
        CONTEXT.instance,
        CONTEXT.allocator
    );

    SM_UNUSED(backend);
    VK_LOG_NOTE( "Vulkan backend shutdown successfully." );
}

internal VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT ,//messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* // pUserData
) {
    usize message_length = str_length( pCallbackData->pMessage );
    const char* message = str_first_non_whitespace_pointer(
        message_length,
        pCallbackData->pMessage
    );
    message_length = str_length( message );
    str_trim_trailing_newline(
        message_length,
        (char*)message
    );

    switch( messageSeverity ) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            VK_LOG_NOTE( "%s", message );
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            VK_LOG_INFO("%s", message);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            VK_LOG_WARN("%s", message);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            VK_LOG_ERROR("%s", message);
        } break;
        default: break;
    }
    return VK_FALSE;
}
