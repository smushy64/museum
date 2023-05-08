/**
 * Description:  Vulkan Renderer Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "vk_defines.h"
#include "vk_backend.h"
#include "platform/platform.h"

global VkContext CONTEXT = {};

#if defined(LD_LOGGING)
    #define REQUIRED_EXTENSIONS_COUNT 1
#else
    #define REQUIRED_EXTENSIONS_COUNT 0
#endif

#if defined(DEBUG)
    #define REQUIRED_LAYER_COUNT     1
#else
    #define REQUIRED_LAYER_COUNT     0
#endif

global const char* REQUIRED_EXTENSIONS_NAMES[REQUIRED_EXTENSIONS_COUNT] = {
#if defined(LD_LOGGING)
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};

global const char* REQUIRED_LAYER_NAMES[REQUIRED_LAYER_COUNT] = {
#if defined(DEBUG)
    VK_LAYER_KHR_VALIDATION_NAME,
#endif
};

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

    instance_info.enabledExtensionCount   = REQUIRED_EXTENSIONS_COUNT;
    instance_info.ppEnabledExtensionNames = REQUIRED_EXTENSIONS_NAMES;

    instance_info.enabledLayerCount   = REQUIRED_LAYER_COUNT;
    instance_info.ppEnabledLayerNames = REQUIRED_LAYER_NAMES;
    
    VkResult instance_result = vkCreateInstance(
        &instance_info,
        CONTEXT.allocator,
        &CONTEXT.instance
    );
    if( instance_result != VK_SUCCESS ) {
        VK_LOG_ERROR(
            "Instance Creation Failed! 0x%X",
            (u32)instance_result
        );
        return false;
    }

    SM_UNUSED(backend);
    SM_UNUSED(platform);
    VK_LOG_NOTE( "Vulkan backend initialized successfully." );
    return true;
}
void vk_shutdown(
    struct RendererBackend* backend
) {
    SM_UNUSED(backend);
    VK_LOG_NOTE( "Vulkan backend shutdown successfully." );
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