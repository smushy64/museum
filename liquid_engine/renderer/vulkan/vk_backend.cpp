/**
 * Description:  Vulkan Renderer Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "vk_backend.h"
#include "platform/platform.h"

b32 vk_init(
    struct RendererBackend* backend,
    const char* app_name,
    struct PlatformState* platform
) {
    return true;
}
void vk_shutdown(
    struct RendererBackend* backend
) {

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