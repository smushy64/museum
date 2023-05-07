/**
 * Description:  Vulkan Renderer Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "vk_defines.h"
#include "vk_backend.h"
#include "platform/platform.h"

SM_GLOBAL VkContext CONTEXT = {};

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
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    
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

    VK_LOG_NOTE( "Vulkan backend initialized successfully." );
    return true;
}
void vk_shutdown(
    struct RendererBackend* backend
) {
    VK_LOG_NOTE( "Vulkan backend shutdown successfully." );
}
void vk_on_resize(
    struct RendererBackend* backend,
    i32 width, i32 height
) {

}
b32 vk_begin_frame(
    struct RendererBackend* backend,
    f32 delta_time
) {
    return true;
}
b32 vk_end_frame(
    struct RendererBackend* backend,
    f32 delta_time
) {
    return true;
}