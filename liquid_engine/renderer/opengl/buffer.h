#if !defined(LD_RENDERER_OPENGL_BUFFER_HPP)
#define LD_RENDERER_OPENGL_BUFFER_HPP
// * Description:  OpenGL Buffer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/mathf/types.h"
#include "renderer/opengl/types.h"

struct packedpad GLDirectionalLight {
    vec4 color;
    vec4 direction;
    mat4 light_space;
};
struct packedpad GLPointLight {
    vec4 color;
    vec4 position;
    mat4 light_space[6];
    f32 is_active;
    f32 near_clip;
    f32 far_clip;
    b32 clipping_planes_set;
};

#define GL_POINT_LIGHT_COUNT (4)
struct packedpad GLLightBuffer {
    struct GLDirectionalLight directional;
    struct GLPointLight       point[GL_POINT_LIGHT_COUNT];
};

#define GL_LIGHT_BUFFER_SIZE    (sizeof(struct GLLightBuffer))
#define GL_LIGHT_BUFFER_BINDING (1)

/// Create a light buffer.
void gl_light_buffer_create(
    GLBufferID buffer_id, struct GLLightBuffer* opt_buffer );
/// Set directional light direction.
void gl_light_buffer_directional_set_direction(
    GLBufferID id, struct GLLightBuffer* buffer, vec3 direction );
/// Set directional light color.
void gl_light_buffer_directional_set_color(
    GLBufferID id, struct GLLightBuffer* buffer, vec3 color );
/// Set all fields of directional light.
void gl_light_buffer_directional_set(
    GLBufferID id, struct GLLightBuffer* buffer,
    vec3 direction, vec3 color );
/// Set point light position.
void gl_light_buffer_point_set_position(
    GLBufferID id, struct GLLightBuffer* buffer, usize index, vec3 position );
/// Set point light color.
void gl_light_buffer_point_set_color(
    GLBufferID id, struct GLLightBuffer* buffer, usize index, vec3 color );
/// Activate point light.
void gl_light_buffer_point_set_active(
    GLBufferID id, struct GLLightBuffer* buffer, usize index, b32 is_active );
/// Set all fields of point light.
void gl_light_buffer_point_set(
    GLBufferID id, struct GLLightBuffer* buffer, usize index,
    vec3 position, vec3 color, b32 is_active );
/// Check if point light is active.
b32 gl_light_buffer_point_is_active(
    struct GLLightBuffer* buffer, usize index );

struct packedpad GLCameraBuffer {
    mat4 matrix_3d;
    mat4 matrix_ui;

    vec4 camera_world_position;

    union {
        struct {
            f32 camera_near;
            f32 camera_far;
        };
        vec4 camera_data;
    };
};

#define GL_CAMERA_BUFFER_OFFSET_MATRIX_3D\
    offsetof(struct GLCameraBuffer, matrix_3d)

#define GL_CAMERA_BUFFER_OFFSET_MATRIX_UI\
    offsetof(struct GLCameraBuffer, matrix_ui)

#define GL_CAMERA_BUFFER_OFFSET_MATRICES\
    (GL_CAMERA_BUFFER_OFFSET_MATRIX_3D)

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_WORLD_POSITION\
    offsetof(struct GLCameraBuffer, camera_world_position)

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_NEAR\
    offsetof(struct GLCameraBuffer, camera_near)

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_FAR\
    offsetof(struct GLCameraBuffer, camera_far)

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_PLANES\
    (GL_CAMERA_BUFFER_OFFSET_CAMERA_NEAR)

#define GL_CAMERA_BUFFER_SIZE    (sizeof(struct GLCameraBuffer))
#define GL_CAMERA_BUFFER_BINDING (0)

/// Make camera buffer
void gl_camera_buffer_create(
    GLBufferID buffer_id, struct GLCameraBuffer* camera );
/// Update all camera buffer fields.
void gl_camera_buffer_update(
    GLBufferID buffer_id, struct GLCameraBuffer* camera );
/// Update 3d view projection matrix.
void gl_camera_buffer_update_matrix_3d(
    GLBufferID buffer_id, mat4* matrix_3d );
/// Update ui view projection matrix.
void gl_camera_buffer_update_matrix_ui(
    GLBufferID buffer_id, mat4* matrix_ui );
/// Update both view projection matrices.
void gl_camera_buffer_update_matrices(
    GLBufferID buffer_id, mat4 matrices[2] );
/// Update camera world position.
void gl_camera_buffer_update_world_position(
    GLBufferID buffer_id, vec3 world_position );
/// Update camera near and far planes.
void gl_camera_buffer_update_near_far_planes(
    GLBufferID buffer_id, f32 planes[2] );

/// Create a framebuffer.
GLFramebuffer gl_framebuffer_create( i32 width, i32 height );
/// Resize a framebuffer
void gl_framebuffer_resize(
    GLFramebuffer* framebuffer, i32 width, i32 height );
/// Destroy framebuffers.
void gl_framebuffer_destroy( usize count, GLFramebuffer* framebuffers );

typedef enum : u32 {
    GL_SHADOWBUFFER_DIRECTIONAL,
    GL_SHADOWBUFFER_POINT
} GLShadowBufferType;

/// Create shadow framebuffer.
GLFramebuffer gl_shadowbuffer_create(
    i32 width, i32 height, GLShadowBufferType type );

#endif // header guard
