#if !defined(CORE_GRAPHICS_HPP)
#define CORE_GRAPHICS_HPP
// * Description:  Graphics
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "core/mathf/types.h"

/// Supported renderer backends
typedef enum RendererBackend : u32 {
    RENDERER_BACKEND_OPENGL,
    RENDERER_BACKEND_VULKAN,
    RENDERER_BACKEND_DX11,
    RENDERER_BACKEND_DX12,

    RENDERER_BACKEND_COUNT
} RendererBackend;
/// Convert renderer backend to const char*
header_only
const char* renderer_backend_to_string( RendererBackend backend ) {
    const char* strings[RENDERER_BACKEND_COUNT] = {
        "OpenGL "
            macro_value_to_string( GL_VERSION_MAJOR ) "."
            macro_value_to_string( GL_VERSION_MINOR ),
        "Vulkan "
            macro_value_to_string( VULKAN_VERSION_MAJOR ) "."
            macro_value_to_string( VULKAN_VERSION_MINOR ),
        "DirectX 11",
        "DirectX 12"
    };
    assert( backend < RENDERER_BACKEND_COUNT );
    return strings[backend];
}
/// Check if renderer backend is supported on the current platform.
header_only
b32 renderer_backend_is_supported( RendererBackend backend ) {
#if !defined(LD_PLATFORM_WINDOWS)
    if(
        backend == RENDERER_BACKEND_DX11 ||
        backend == RENDERER_BACKEND_DX12
    ) {
        return false;
    }
#endif
    unused(backend);
    return true;
}

#endif // header guard
