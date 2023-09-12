#if !defined(LD_CORE_MATH_TYPES_H)
#define LD_CORE_MATH_TYPES_H
// * Description:  Ada Math types
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 03, 2023
#include "defines.h"

#if !defined(LD_CORE_MATH_TYPES_TYPEDEF)
#define LD_CORE_MATH_TYPES_TYPEDEF
/// 2-component 32-bit float vector
typedef union vec2 vec2;
/// 3-component 32-bit float vector
typedef union vec3 vec3;
/// RGB color.
typedef union vec3 rgb;
/// HSV color.
typedef union vec3 hsv;
/// 3D rotation expressed in euler angles.
typedef union vec3 euler_angles;
/// 4-component 32-bit float vector
typedef union vec4 vec4;
/// RGBA color.
typedef union vec4 rgba;

/// 2-component 32-bit integer vector
typedef union ivec2 ivec2;
/// 3-component 32-bit integer vector
typedef union ivec3 ivec3;
/// 4-component 32-bit integer vector
typedef union ivec4 ivec4;

/// Quaternion.
typedef union quat quat;

/// Column-major 2x2 32-bit float matrix.
typedef union mat2 mat2;
/// Column-major 3x3 32-bit float matrix.
typedef union mat3 mat3;
/// Column-major 4x4 32-bit float matrix.
typedef union mat4 mat4;

/// Transform.
typedef struct Transform Transform;
#endif

#define VEC2_COMPONENT_COUNT (2)
/// 2-component 32-bit float vector
union vec2 {
    struct { f32 x, y; };
    struct { f32 u, v; };
    struct { f32 width, height; };
    f32 c[VEC2_COMPONENT_COUNT];
};
#define VEC2_ZERO  (vec2){}
#define VEC2_ONE   (vec2){  1.0f,  1.0f }
#define VEC2_LEFT  (vec2){ -1.0f,  0.0f }
#define VEC2_RIGHT (vec2){  1.0f,  0.0f }
#define VEC2_UP    (vec2){  0.0f,  1.0f }
#define VEC2_DOWN  (vec2){  0.0f, -1.0f }

/// Create zero vector.
header_only vec2 v2_zero(void) { return VEC2_ZERO; }
/// Create vector with all components set to given scalar.
header_only vec2 v2_scalar( f32 scalar ) {
    vec2 result = {scalar, scalar};
    return result;
}
/// Create vector with given components.
header_only vec2 v2( f32 x, f32 y ) {
    vec2 result = { x, y };
    return result;
}

#define IVEC2_COMPONENT_COUNT (2)
/// 2-component 32-bit signed integer vector
union ivec2 {
    struct { i32 x, y; };
    struct { i32 u, v; };
    struct { i32 width, height; };
    i32 c[IVEC2_COMPONENT_COUNT];
};
#define IVEC2_ZERO  (ivec2){} 
#define IVEC2_ONE   (ivec2){  1,  1 } 
#define IVEC2_LEFT  (ivec2){ -1,  0 } 
#define IVEC2_RIGHT (ivec2){  1,  0 } 
#define IVEC2_UP    (ivec2){  0,  1 } 
#define IVEC2_DOWN  (ivec2){  0, -1 } 

/// Create zero vector.
header_only ivec2 iv2_zero(void) { return IVEC2_ZERO; }
/// Create vector with all components set to given scalar.
header_only ivec2 iv2_scalar( i32 scalar ) {
    ivec2 result = { scalar, scalar };
    return result;
}
/// Create vector with given components.
header_only ivec2 iv2( i32 x, i32 y ) {
    ivec2 result = { x, y };
    return result;
}

#define VEC3_COMPONENT_COUNT (3)
/// 3-component 32-bit float vector
union vec3 {
    struct {
        union {
            struct { f32 x, y; };
            struct { f32 r, g; };
            struct { f32 hue, value; };
            struct { f32 width, height; };
            struct { f32 pitch, yaw; };
            vec2 xy;
            vec2 rg;
        };
        union {
            f32 z;
            f32 b;
            f32 saturation;
            f32 length;
            f32 roll;
        };
    };
    f32 c[VEC3_COMPONENT_COUNT];
};

#define VEC3_ZERO    (vec3){}
#define VEC3_ONE     (vec3){  1.0f,  1.0f,  1.0f } 
#define VEC3_LEFT    (vec3){ -1.0f,  0.0f,  0.0f } 
#define VEC3_RIGHT   (vec3){  1.0f,  0.0f,  0.0f } 
#define VEC3_UP      (vec3){  0.0f,  1.0f,  0.0f } 
#define VEC3_DOWN    (vec3){  0.0f, -1.0f,  0.0f } 
#define VEC3_FORWARD (vec3){  0.0f,  0.0f,  1.0f } 
#define VEC3_BACK    (vec3){  0.0f,  0.0f, -1.0f } 

#define RGB_RED     (rgb){ 1.0f, 0.0f, 0.0f } 
#define RGB_GREEN   (rgb){ 0.0f, 1.0f, 0.0f } 
#define RGB_BLUE    (rgb){ 0.0f, 0.0f, 1.0f } 
#define RGB_MAGENTA (rgb){ 1.0f, 0.0f, 1.0f } 
#define RGB_YELLOW  (rgb){ 1.0f, 1.0f, 0.0f } 
#define RGB_CYAN    (rgb){ 0.0f, 1.0f, 1.0f } 
#define RGB_BLACK   (rgb){ 0.0f, 0.0f, 0.0f } 
#define RGB_WHITE   (rgb){ 1.0f, 1.0f, 1.0f } 
#define RGB_GRAY    (rgb){ 0.5f, 0.5f, 0.5f } 
#define RGB_GREY    (rgb){ 0.5f, 0.5f, 0.5f } 

/// Create zero vector.
header_only vec3 v3_zero(void) { return VEC3_ZERO; }
/// Create vector with given components.
header_only vec3 v3( f32 x, f32 y, f32 z ) {
    vec3 result = { x, y, z };
    return result;
}
/// Create vector with all components set to given scalar.
header_only vec3 v3_scalar( f32 scalar ) {
    vec3 result = { scalar, scalar, scalar };
    return result;
}

#define IVEC3_COMPONENT_COUNT (3)
/// 3-component 32-bit signed integer vector
union ivec3 {
    struct {
        union {
            struct { i32 x, y; };
            struct { i32 width, height; };
            ivec2 xy;
        };
        union {
            i32 z;
            i32 depth;
        };
    };
    i32 c[IVEC3_COMPONENT_COUNT];
};

#define IVEC3_ZERO    (ivec3){  0,  0,  0 } 
#define IVEC3_ONE     (ivec3){  1,  1,  1 } 
#define IVEC3_LEFT    (ivec3){ -1,  0,  0 } 
#define IVEC3_RIGHT   (ivec3){  1,  0,  0 } 
#define IVEC3_UP      (ivec3){  0,  1,  0 } 
#define IVEC3_DOWN    (ivec3){  0, -1,  0 } 
#define IVEC3_FORWARD (ivec3){  0,  0,  1 } 
#define IVEC3_BACK    (ivec3){  0,  0, -1 } 

/// Create zero vector.
header_only ivec3 iv3_zero(void) { return IVEC3_ZERO; }
/// Create vector with all components set to given scalar.
header_only ivec3 iv3_scalar( i32 scalar ) {
    ivec3 result = { scalar, scalar, scalar };
    return result;
}
/// Create vector with given components.
header_only ivec3 iv3( i32 x, i32 y, i32 z ) {
    ivec3 result = { x, y, z };
    return result;
}

#define VEC4_COMPONENT_COUNT (4)
/// 4-component 32-bit float vector
union vec4 {
    struct {
        union {
            struct {
                union {
                    struct { f32 x, y; };
                    struct { f32 r, g; };
                    vec2 xy;
                    vec2 rg;
                };
                union { f32 z; f32 b; };
            };
            vec3 xyz;
            vec3 rgb;
        };
        union { f32 w; f32 a; };
    };
    f32 c[VEC4_COMPONENT_COUNT];
};

#define VEC4_ZERO (vec4){}
#define VEC4_ONE  (vec4){ 1.0f, 1.0f, 1.0f, 1.0f }

#define RGBA_RED     (rgba){ 1.0f, 0.0f, 0.0f, 1.0f } 
#define RGBA_GREEN   (rgba){ 0.0f, 1.0f, 0.0f, 1.0f } 
#define RGBA_BLUE    (rgba){ 0.0f, 0.0f, 1.0f, 1.0f } 
#define RGBA_MAGENTA (rgba){ 1.0f, 0.0f, 1.0f, 1.0f } 
#define RGBA_YELLOW  (rgba){ 1.0f, 1.0f, 0.0f, 1.0f } 
#define RGBA_CYAN    (rgba){ 0.0f, 1.0f, 1.0f, 1.0f } 
#define RGBA_BLACK   (rgba){ 0.0f, 0.0f, 0.0f, 1.0f } 
#define RGBA_WHITE   (rgba){ 1.0f, 1.0f, 1.0f, 1.0f } 
#define RGBA_GRAY    (rgba){ 0.5f, 0.5f, 0.5f, 1.0f } 
#define RGBA_GREY    (rgba){ 0.5f, 0.5f, 0.5f, 1.0f } 
#define RGBA_CLEAR   (rgba){ 0.0f, 0.0f, 0.0f, 0.0f } 

/// Create zero vector.
header_only vec4 v4_zero(void) { return VEC4_ZERO; }
/// Create vector with given components.
header_only vec4 v4( f32 x, f32 y, f32 z, f32 w ) {
    vec4 result = { x, y, z, w };
    return result;
}
/// Create vector with all components set to given scalar.
header_only vec4 v4_scalar( f32 scalar ) {
    vec4 result = { scalar, scalar, scalar, scalar };
    return result;
}

#define IVEC4_COMPONENT_COUNT (4)
/// 4-component 32-bit signed integer vector
union ivec4 {
    struct {
        union {
            struct {
                union {
                    struct { i32 x, y; };
                    ivec2 xy;
                };
                i32 z;
            };
            ivec3 xyz;
        };
        i32 w;
    };
    i32 c[IVEC4_COMPONENT_COUNT];
};

#define IVEC4_ZERO (ivec4){} 
#define IVEC4_ONE  (ivec4){ 1, 1, 1, 1 } 

/// Create vector with given components.
header_only ivec4 iv4( i32 x, i32 y, i32 z, i32 w ) {
    ivec4 result = { x, y, z, w };
    return result;
}
/// Create zero vector.
header_only ivec4 iv4_zero(void) { return IVEC4_ZERO; }
/// Create vector with all components set to given scalar.
header_only ivec4 iv4_scalar( i32 scalar ) {
    ivec4 result = { scalar, scalar, scalar, scalar };
    return result;
}

#define QUAT_COMPONENT_COUNT (4)
/// Quaternion.
union quat {
    struct {
        union {
            f32 w;
            f32 a;
        };
        union {
            struct { f32 x, y, z; };
            struct { f32 b, c, d; };
            vec3 xyz;
        };
    };
    f32 q[QUAT_COMPONENT_COUNT];
};

#define QUAT_ZERO     (quat){ 0.0f, 0.0f, 0.0f, 0.0f } 
#define QUAT_IDENTITY (quat){ 1.0f, 0.0f, 0.0f, 0.0f } 

/// Create zero quaternion.
header_only quat q_zero(void) { return QUAT_ZERO; }
/// Create quaternion with set to given scalars.
header_only quat q( f32 w, f32 x, f32 y, f32 z ) {
    quat result = { w, x, y, z };
    return result;
}

#define MAT2_CELL_COUNT   (4)
#define MAT2_COLUMN_COUNT (2)
#define MAT2_ROW_COUNT    (2)
/// Column-major 2x2 32-bit float matrix.
union mat2 {
    struct {
        union {
            struct { f32 m00, m01; };
            vec2 col0;
        };
        union {
            struct { f32 m10, m11; };
            vec2 col1;
        };
    };
    vec2 v[MAT2_COLUMN_COUNT];
    f32  c[MAT2_CELL_COUNT];
    f32  m[MAT2_COLUMN_COUNT][MAT2_ROW_COUNT];
};

#define MAT2_ZERO (mat2){\
    0.0f, 0.0f,\
    0.0f, 0.0f\
}
#define MAT2_IDENTITY (mat2){\
    1.0f, 0.0f,\
    0.0f, 1.0f\
}

/// Create zero matrix.
header_only mat2 m2_zero(void) { return MAT2_ZERO; }
/// Create matrix with given values.
header_only mat2 m2(
    f32 m00, f32 m01,
    f32 m10, f32 m11
) {
    mat2 result = { m00, m01, m10, m11 };
    return result;
}

#define MAT3_CELL_COUNT   (9)
#define MAT3_COLUMN_COUNT (3)
#define MAT3_ROW_COUNT    (3)
/// Column-major 3x3 32-bit float matrix.
union mat3 {
    struct {
        union {
            struct { f32 m00, m01, m02; };
            vec3 col0;
        };
        union {
            struct { f32 m10, m11, m12; };
            vec3 col1;
        };
        union {
            struct { f32 m20, m21, m22; };
            vec3 col2;
        };
    };
    vec3 v[MAT3_COLUMN_COUNT];
    f32  c[MAT3_CELL_COUNT];
    f32  m[MAT3_COLUMN_COUNT][MAT3_ROW_COUNT];
};

#define MAT3_ZERO (mat3){\
    0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f\
}
#define MAT3_IDENTITY (mat3){\
    1.0f, 0.0f, 0.0f,\
    0.0f, 1.0f, 0.0f,\
    0.0f, 0.0f, 1.0f\
}

/// Create zero matrix.
header_only mat3 m3_zero(void) { return MAT3_ZERO; }
/// Create matrix from given values.
header_only mat3 m3(
    f32 m00, f32 m01, f32 m02,
    f32 m10, f32 m11, f32 m12,
    f32 m20, f32 m21, f32 m22
) {
    mat3 result = {
        m00, m01, m02,
        m10, m11, m12,
        m20, m21, m22
    };
    return result;
}

#define MAT4_CELL_COUNT   (16)
#define MAT4_COLUMN_COUNT (4)
#define MAT4_ROW_COUNT    (4)
/// Column-major 4x4 32-bit float matrix.
union mat4 {
    struct {
        union {
            struct { f32 m00, m01, m02, m03; };
            vec4 col0;
        };
        union {
            struct { f32 m10, m11, m12, m13; };
            vec4 col1;
        };
        union {
            struct { f32 m20, m21, m22, m23; };
            vec4 col2;
        };
        union {
            struct { f32 m30, m31, m32, m33; };
            vec4 col3;
        };
    };
    vec4 v[MAT4_COLUMN_COUNT];
    f32  c[MAT4_CELL_COUNT];
    f32  m[MAT4_COLUMN_COUNT][MAT4_ROW_COUNT];
};

#define MAT4_ZERO (mat4){\
    0.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 0.0f\
}
#define MAT4_IDENTITY (mat4){\
    1.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 1.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 1.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 1.0f\
}

/// Create a zero matrix.
header_only mat4 m4_zero(void) { return MAT4_ZERO; }
/// Create a matrix from given values.
header_only mat4 m4(
    f32 m00, f32 m01, f32 m02, f32 m03,
    f32 m10, f32 m11, f32 m12, f32 m13,
    f32 m20, f32 m21, f32 m22, f32 m23,
    f32 m30, f32 m31, f32 m32, f32 m33
) {
    mat4 result = {
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    };
    return result;
}

/// Transform.
/// You should never directly modify any
/// of the transform's components!
struct Transform {
    vec3 position;
    quat rotation;
    vec3 scale;
    struct { b16 matrix_dirty, camera_dirty; };

    mat4 matrix;
    struct Transform* parent;
};

/// Create a transform.
header_only Transform transform_create(
    vec3 position, quat rotation, vec3 scale
) {
    Transform result;
    result.position = position;
    result.rotation = rotation;
    result.scale    = scale;

    result.matrix       = MAT4_IDENTITY;
    result.matrix_dirty = true;
    result.camera_dirty = true;
    result.parent       = NULL;

    return result;
}
/// Create a default transform.
header_only Transform transform_zero(void) {
    return transform_create( v3_zero(), QUAT_IDENTITY, VEC3_ONE );
}
/// Create a transform with defaults except for position.
header_only Transform transform_with_position( vec3 position ) {
    return transform_create( position, QUAT_IDENTITY, VEC3_ONE );
}
/// Create a transform with defaults except for rotation.
header_only Transform transform_with_rotation( quat rotation ) {
    return transform_create( VEC3_ZERO, rotation, VEC3_ONE );
}
/// Create a transform with defaults except for scale.
header_only Transform transform_with_scale( vec3 scale ) {
    return transform_create( VEC3_ZERO, QUAT_IDENTITY, scale );
}

/// Create vec2 from vec3.
header_only vec2 v2_v3( vec3 v ) {
    return v2( v.x, v.y );
}
/// Create vec2 from vec4.
header_only vec2 v2_v4( vec4 v ) {
    return v2( v.x, v.y );
}
/// Create vec2 from ivec2.
header_only vec2 v2_iv2( ivec2 v ) {
    return v2( v.x, v.y );
}

/// Create ivec2 from ivec3.
header_only ivec2 iv2_iv3( ivec3 v ) {
    return iv2( v.x, v.y );
}
/// Create ivec2 from ivec4.
header_only ivec2 iv2_iv4( ivec4 v ) {
    return iv2( v.x, v.y );
}
/// Create ivec2 from vec2.
header_only ivec2 iv2_v2( vec2 v ) {
    return iv2( v.x, v.y );
}

/// Create vec3 from vec2.
header_only vec3 v3_v2( vec2 v ) {
    return v3( v.x, v.y, 0.0f );
}
/// Create vec3 from vec4.
header_only vec3 v3_v4( vec4 v ) {
    return v3( v.x, v.y, v.z );
}
/// Create vec3 from ivec3.
header_only vec3 v3_iv3( ivec3 v ) {
    return v3( v.x, v.y, v.z );
}
/// Create rgb from rgba.
header_only rgb rgb_rgba( rgba col ) {
    return v3( col.r, col.g, col.b );
}

/// Create ivec3 from ivec2.
header_only ivec3 iv3_iv2( ivec2 v ) {
    return iv3( v.x, v.y, 0 );
}
/// Create ivec3 from ivec4.
header_only ivec3 iv3_iv4( ivec4 v ) {
    return iv3( v.x, v.y, v.z );
}
/// Create ivec3 from vec3.
header_only ivec3 iv3_v3( vec3 v ) {
    return iv3( v.x, v.y, v.z );
}

/// Create vec4 from vec2.
header_only vec4 v4_v2( vec2 v ) {
    return v4( v.x, v.y, 0.0f, 0.0f );
}
/// Create vec4 from vec3.
header_only vec4 v4_v3( vec3 v ) {
    return v4( v.x, v.y, v.z, 0.0f );
}
/// Create vec4 from quat.
header_only vec4 v4_q( quat q ) {
    return v4( q.a, q.b, q.c, q.d );
}
/// Create RGBA from RGB. Alpha set to 1
header_only rgba rgba_rgb( rgb col ) {
    return v4( col.r, col.g, col.b, 1.0f );
}
/// Create vec4 from ivec4.
header_only vec4 v4_iv4( ivec4 v ) {
    return v4( v.x, v.y, v.z, v.w );
}

/// Create ivec4 from ivec2.
header_only ivec4 iv4_iv2( ivec2 v ) {
    return iv4( v.x, v.y, 0, 0 );
}
/// Create ivec4 from ivec3.
header_only ivec4 iv4_iv3( ivec3 v ) {
    return iv4( v.x, v.y, v.z, 0 );
}
/// Create ivec4 from vec4.
header_only ivec4 iv4_v4( vec4 v ) {
    return iv4( v.x, v.y, v.z, v.w );
}

/// Create euler angles from quaternion.
LD_API euler_angles euler_q( quat q );

/// Create mat2 from mat3.
LD_API mat2 m2_m3( const mat3* m );
/// Create mat2 from mat4.
LD_API mat2 m2_m4( const mat4* m );

/// Create mat3 from mat2.
header_only mat3 m3_m2( mat2 m ) {
    return (mat3){
        m.c[0], m.c[1], 0.0f,
        m.c[2], m.c[3], 0.0f,
          0.0f,   0.0f, 0.0f
    };
}
/// Create mat3 from mat4.
LD_API mat3 m3_m4( const mat4* m );

/// Create mat4 from mat2.
header_only mat4 m4_m2( mat2 m ) {
    return (mat4){
        m.c[0], m.c[1], 0.0f, 0.0f,
        m.c[2], m.c[3], 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
    };
}
/// Create mat4 from mat3.
LD_API mat4 m4_m3( const mat3* m );

#endif // header guard
