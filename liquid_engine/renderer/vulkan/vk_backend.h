#if !defined(VK_BACKEND_HPP)
#define VK_BACKEND_HPP
/**
 * Description:  Vulkan Renderer Backend
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "defines.h"
#include "renderer/renderer_defines.h"

b32 vk_init(
    struct RendererBackend* backend,
    const char* app_name,
    struct PlatformState* platform
);
void vk_shutdown(
    struct RendererBackend* backend
);
void vk_on_resize(
    struct RendererBackend* backend,
    i32 width, i32 height
);
b32 vk_begin_frame(
    struct RendererBackend* backend,
    f32 delta_time
);
b32 vk_end_frame(
    struct RendererBackend* backend,
    f32 delta_time
);

#endif // header guard