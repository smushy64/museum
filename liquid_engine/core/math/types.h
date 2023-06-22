#if !defined(CORE_MATH_TYPES_HPP)
#define CORE_MATH_TYPES_HPP
/**
 * Description:  Math types
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 08, 2023
*/
#include "defines.h"

#if defined(LD_COMPILER_CLANG)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-braces"
#endif

union vec2;
union ivec2;

union vec3;
union ivec3;

typedef vec3 rgb;
typedef vec3 hsv;
typedef vec3 euler_angles;

union vec4;
union ivec4;

typedef vec4 rgba;

struct angle_axis;
union  quat;

union mat2;
union mat3;
union mat4;

namespace VEC2 {
    global const usize COMPONENT_COUNT = 2;
};
inline vec2 operator+( vec2 lhs, vec2 rhs );
inline vec2 operator-( vec2 lhs, vec2 rhs );
inline vec2 operator*( vec2 lhs, f32 rhs );
inline vec2 operator*( f32 lhs, vec2 rhs );
inline vec2 operator/( vec2 lhs, f32 rhs );
/// 2-component 32-bit float vector
union vec2 {
    struct { f32 x, y; };
    struct { f32 u, v; };
    struct { f32 width, height; };
    f32 c[VEC2::COMPONENT_COUNT];

    inline f32& operator[](u32 index)       { return c[index]; }
    inline f32  operator[](u32 index) const { return c[index]; }

    inline b32 operator==( vec2 b ) const {
        vec2 tmp = *this - b;
        return (tmp.x * tmp.x + tmp.y * tmp.y) < F32::EPSILON;
    }
    inline b32 operator!=( vec2 b ) const {
        return !(*this == b);
    }

    inline vec2 operator-() const { return { -this->x, -this->y }; }
    inline vec2& operator+=( vec2 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    inline vec2& operator-=( vec2 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    inline vec2& operator*=( f32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    inline vec2& operator/=( f32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
namespace VEC2 {
    /// { 1.0, 1.0 }
    global const vec2 ONE   = {  1.0f,  1.0f };
    /// { 0.0, 0.0 }
    global const vec2 ZERO  = {  0.0f,  0.0f };
    /// { -1.0, 0.0 }
    global const vec2 LEFT  = { -1.0f,  0.0f };
    /// { 1.0, 0.0 }
    global const vec2 RIGHT = {  1.0f,  0.0f };
    /// { 0.0, 1.0 }
    global const vec2 UP    = {  0.0f,  1.0f };
    /// { 0.0, -1.0 }
    global const vec2 DOWN  = {  0.0f, -1.0f };
} // namespace VEC2

/// Pointer to vector value.
inline f32* value_pointer( const vec2& v ) { return (f32*)&v.x; }
/// Create zero vector.
inline vec2 v2() { return {}; }
/// Create vector with all components set to given scalar.
inline vec2 v2( f32 scalar ) { return { scalar, scalar }; }
/// Create vector with given components.
inline vec2 v2( f32 x, f32 y ) { return { x, y }; }
inline vec2 operator+( vec2 lhs, vec2 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}
inline vec2 operator-( vec2 lhs, vec2 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}
inline vec2 operator*( vec2 lhs, f32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs
    };
}
inline vec2 operator*( f32 lhs, vec2 rhs ) {
    return operator*( rhs, lhs );
}
inline vec2 operator/( vec2 lhs, f32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs
    };
}

namespace IVEC2 {
    global const usize COMPONENT_COUNT = 2;
};

inline ivec2 operator+( ivec2 lhs, ivec2 rhs );
inline ivec2 operator-( ivec2 lhs, ivec2 rhs );
inline ivec2 operator*( ivec2 lhs, i32 rhs );
inline ivec2 operator*( i32 lhs, ivec2 rhs );
inline ivec2 operator/( ivec2 lhs, i32 rhs );
/// 2-component 32-bit signed integer vector
union ivec2 {
    struct { i32 x, y; };
    struct { i32 u, v; };
    struct { i32 width, height; };
    i32 c[IVEC2::COMPONENT_COUNT];

    inline i32& operator[](u32 index)       { return c[index]; }
    inline i32  operator[](u32 index) const { return c[index]; }

    inline b32 operator==( ivec2 b ) const {
        return
            this->x == b.x &&
            this->y == b.y;
    }
    inline b32 operator!=( ivec2 b ) const { return !(*this == b); }

    inline ivec2 operator-() const { return { -this->x, -this->y }; }
    inline ivec2& operator+=( ivec2 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    inline ivec2& operator-=( ivec2 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    inline ivec2& operator*=( i32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    inline ivec2& operator/=( i32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
namespace IVEC2 {
    /// { 1, 1 }
    global const ivec2 ONE   = {  1,  1 };
    /// { 0, 0 }
    global const ivec2 ZERO  = {  0,  0 };
    /// { -1, 0 }
    global const ivec2 LEFT  = { -1,  0 };
    /// { 1, 0 }
    global const ivec2 RIGHT = {  1,  0 };
    /// { 0, 1 }
    global const ivec2 UP    = {  0,  1 };
    /// { 0, -1 }
    global const ivec2 DOWN  = {  0, -1 };
} // namespace smath::VEC2

/// Pointer to vector value.
inline i32* value_pointer( const ivec2& v ) { return (i32*)&v.x; }
/// Create zero vector.
inline ivec2 iv2() { return {}; }
/// Create vector with all components set to given scalar.
inline ivec2 iv2( i32 scalar ) { return { scalar, scalar }; }
/// Create vector with given components.
inline ivec2 iv2( i32 x, i32 y ) { return { x, y }; }
inline ivec2 operator+( ivec2 lhs, ivec2 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}
inline ivec2 operator-( ivec2 lhs, ivec2 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}
inline ivec2 operator*( ivec2 lhs, i32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs
    };
}
inline ivec2 operator*( i32 lhs, ivec2 rhs ) {
    return operator*( rhs, lhs );
}
inline ivec2 operator/( ivec2 lhs, i32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs
    };
}

namespace VEC3 {
    global const usize COMPONENT_COUNT = 3;
};
inline vec3 operator+( vec3 lhs, vec3 rhs );
inline vec3 operator-( vec3 lhs, vec3 rhs );
inline vec3 operator*( vec3 lhs, f32 rhs );
inline vec3 operator*( f32 lhs, vec3 rhs );
inline vec3 operator/( vec3 lhs, f32 rhs );
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
    f32 c[VEC3::COMPONENT_COUNT];

    inline f32& operator[](u32 index)       { return c[index]; }
    inline f32  operator[](u32 index) const { return c[index]; }

    inline b32 operator==( vec3 b ) const {
        vec3 tmp = *this - b;
        return (
            tmp.x * tmp.x +
            tmp.y * tmp.y +
            tmp.z * tmp.z
        ) < F32::EPSILON;
    }
    inline b32 operator!=( vec3 b ) const { return !(*this == b); }

    inline vec3 operator-() const {
        return { -this->x, -this->y, -this->z };
    }
    inline vec3& operator+=( vec3 rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    inline vec3& operator-=( vec3 rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    inline vec3& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    inline vec3& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
namespace VEC3 {
    /// { 1.0, 1.0, 1.0 }
    global const vec3 ONE     = {  1.0f,  1.0f,  1.0f };
    /// { 0.0, 0.0, 0.0 }
    global const vec3 ZERO    = {  0.0f,  0.0f,  0.0f };
    /// { -1.0, 0.0, 0.0 }
    global const vec3 LEFT    = { -1.0f,  0.0f,  0.0f };
    /// { 1.0, 0.0, 0.0 }
    global const vec3 RIGHT   = {  1.0f,  0.0f,  0.0f };
    /// { 0.0, 1.0, 0.0 }
    global const vec3 UP      = {  0.0f,  1.0f,  0.0f };
    /// { 0.0, -1.0, 0.0 }
    global const vec3 DOWN    = {  0.0f, -1.0f,  0.0f };
    /// { 0.0, 0.0, 1.0 }
    global const vec3 FORWARD = {  0.0f,  0.0f,  1.0f };
    /// { 0.0, 0.0, -1.0 }
    global const vec3 BACK    = {  0.0f,  0.0f, -1.0f };
} // namespace VEC3

namespace RGB {
    /// { 1.0, 0.0, 0.0 }
    global const rgb RED     = { 1.0f, 0.0f, 0.0f };
    /// { 0.0, 1.0, 0.0 }
    global const rgb GREEN   = { 0.0f, 1.0f, 0.0f };
    /// { 0.0, 0.0, 1.0 }
    global const rgb BLUE    = { 0.0f, 0.0f, 1.0f };
    /// { 1.0, 0.0, 1.0 }
    global const rgb MAGENTA = { 1.0f, 0.0f, 1.0f };
    /// { 1.0, 1.0, 0.0 }
    global const rgb YELLOW  = { 1.0f, 1.0f, 0.0f };
    /// { 0.0, 1.0, 1.0 }
    global const rgb CYAN    = { 0.0f, 1.0f, 1.0f };
    /// { 0.0, 0.0, 0.0 }
    global const rgb BLACK   = { 0.0f, 0.0f, 0.0f };
    /// { 1.0, 1.0, 1.0 }
    global const rgb WHITE   = { 1.0f, 1.0f, 1.0f };
    /// { 0.5, 0.5, 0.5 }
    global const rgb GRAY    = { 0.5f, 0.5f, 0.5f };
    /// { 0.5, 0.5, 0.5 }
    global const rgb GREY    = { 0.5f, 0.5f, 0.5f };
} // namespace RGB

/// Pointer to vector value.
inline f32* value_pointer( const vec3& v ) { return (f32*)&v.x; }
/// Create zero vector.
inline vec3 v3() { return {}; }
/// Create vector with all components set to given scalar.
inline vec3 v3( f32 scalar ) {
    return { scalar, scalar, scalar };
}
/// Create vector with given components.
inline vec3 v3( f32 x, f32 y, f32 z ) {
    return { x, y, z };
}

inline vec3 operator+( vec3 lhs, vec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline vec3 operator-( vec3 lhs, vec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline vec3 operator*( vec3 lhs, f32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline vec3 operator*( f32 lhs, vec3 rhs ) {
    return operator*( rhs, lhs );
}
inline vec3 operator/( vec3 lhs, f32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}

namespace IVEC3 {
    global const usize COMPONENT_COUNT = 3;
};

inline ivec3 operator+( ivec3 lhs, ivec3 rhs );
inline ivec3 operator-( ivec3 lhs, ivec3 rhs );
inline ivec3 operator*( ivec3 lhs, i32 rhs );
inline ivec3 operator*( i32 lhs, ivec3 rhs );
inline ivec3 operator/( ivec3 lhs, i32 rhs );
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
    i32 c[IVEC3::COMPONENT_COUNT];

    inline i32& operator[](u32 index)       { return c[index]; }
    inline i32  operator[](u32 index) const { return c[index]; }

    inline b32 operator==( ivec3 b ) const {
        return
            this->x == b.x &&
            this->y == b.y &&
            this->z == b.z;
    }
    inline b32 operator!=( ivec3 b ) const { return !(*this == b); }

    inline ivec3 operator-() const { return { -this->x, -this->y, -this->z }; }
    inline ivec3& operator+=( ivec3 rhs ) {
        return *this = operator+(*this, rhs);
    }
    inline ivec3& operator-=( ivec3 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    inline ivec3& operator*=( i32 rhs ) {
        return *this = operator*(*this, rhs);
    }
    inline ivec3& operator/=( i32 rhs ) {
        return *this = operator/(*this, rhs);
    }
};
namespace IVEC3 {
    /// { 1, 1, 1 }
    global const ivec3 ONE     = {  1,  1,  1 };
    /// { 0, 0, 0 }
    global const ivec3 ZERO    = {  0,  0,  0 };
    /// { -1, 0, 0 }
    global const ivec3 LEFT    = { -1,  0,  0 };
    /// { 1, 0, 0 }
    global const ivec3 RIGHT   = {  1,  0,  0 };
    /// { 0, 1, 0 }
    global const ivec3 UP      = {  0,  1,  0 };
    /// { 0, -1, 0 }
    global const ivec3 DOWN    = {  0, -1,  0 };
    /// { 0, 0, 1 }
    global const ivec3 FORWARD = {  0,  0,  1 };
    /// { 0, 0, -1 }
    global const ivec3 BACK    = {  0,  0, -1 };
} // namespace IVEC3

/// Pointer to vector value.
inline i32* value_pointer( const ivec3& v ) { return (i32*)&v.x; }
/// Create zero vector.
inline ivec3 iv3() { return {}; }
/// Create vector with all components set to given scalar.
inline ivec3 iv3( i32 scalar ) { return { scalar, scalar, scalar }; }
/// Create vector with given components.
inline ivec3 iv3( i32 x, i32 y, i32 z ) { return { x, y, z }; }

inline ivec3 operator+( ivec3 lhs, ivec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline ivec3 operator-( ivec3 lhs, ivec3 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}
inline ivec3 operator*( ivec3 lhs, i32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline ivec3 operator*( i32 lhs, ivec3 rhs ) {
    return operator*( rhs, lhs );
}
inline ivec3 operator/( ivec3 lhs, i32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}

namespace VEC4 {
    global const usize COMPONENT_COUNT = 4;
};

inline vec4 operator+( vec4 lhs, vec4 rhs );
inline vec4 operator-( vec4 lhs, vec4 rhs );
inline vec4 operator*( vec4 lhs, f32 rhs );
inline vec4 operator*( f32 lhs, vec4 rhs );
inline vec4 operator/( vec4 lhs, f32 rhs );
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
                union {
                    f32 z;
                    f32 b;
                };
            };
            vec3 xyz;
            vec3 rgb;
        };
        union {
            f32 w;
            f32 a;
        };
    };
    f32 c[VEC4::COMPONENT_COUNT];

    inline f32& operator[](u32 index)       { return c[index]; }
    inline f32  operator[](u32 index) const { return c[index]; }

    inline b32 operator==( vec4 b ) const {
        vec4 tmp = *this - b;
        return (
            tmp.x * tmp.x +
            tmp.y * tmp.y +
            tmp.z * tmp.z +
            tmp.w * tmp.w
        ) < F32::EPSILON;
    }
    inline b32 operator!=( vec4 rhs ) const { return !(*this == rhs); }

    inline vec4 operator-() const {
        return { -this->x, -this->y, -this->z, -this->w };
    }
    inline vec4& operator+=( vec4 rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    inline vec4& operator-=( vec4 rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    inline vec4& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    inline vec4& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
namespace VEC4 {
    /// { 1.0, 1.0, 1.0, 1.0 }
    global const vec4 ONE  = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// { 0.0, 0.0, 0.0, 0.0 }
    global const vec4 ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
} // namespace VEC4

namespace RGBA {
    /// { 1.0, 0.0, 0.0, 1.0 }
    global const rgba RED     = { 1.0f, 0.0f, 0.0f, 1.0f };
    /// { 0.0, 1.0, 0.0, 1.0 }
    global const rgba GREEN   = { 0.0f, 1.0f, 0.0f, 1.0f };
    /// { 0.0, 0.0, 1.0, 1.0 }
    global const rgba BLUE    = { 0.0f, 0.0f, 1.0f, 1.0f };
    /// { 1.0, 0.0, 1.0, 1.0 }
    global const rgba MAGENTA = { 1.0f, 0.0f, 1.0f, 1.0f };
    /// { 1.0, 1.0, 0.0, 1.0 }
    global const rgba YELLOW  = { 1.0f, 1.0f, 0.0f, 1.0f };
    /// { 0.0, 1.0, 1.0, 1.0 }
    global const rgba CYAN    = { 0.0f, 1.0f, 1.0f, 1.0f };
    /// { 0.0, 0.0, 0.0, 1.0 }
    global const rgba BLACK   = { 0.0f, 0.0f, 0.0f, 1.0f };
    /// { 1.0, 1.0, 1.0, 1.0 }
    global const rgba WHITE   = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// { 0.5, 0.5, 0.5, 1.0 }
    global const rgba GRAY    = { 0.5f, 0.5f, 0.5f, 1.0f };
    /// { 0.5, 0.5, 0.5, 1.0 }
    global const rgba GREY    = { 0.5f, 0.5f, 0.5f, 1.0f };
    /// { 0.0, 0.0, 0.0, 0.0 }
    global const rgba CLEAR   = { 0.0f, 0.0f, 0.0f, 0.0f };
} // namespace RGBA

/// Pointer to vector value.
inline f32* value_pointer( const vec4& v ) { return (f32*)&v.x; }
/// Create zero vector.
inline vec4 v4() { return {}; }
/// Create vector with all components set to given scalar.
inline vec4 v4( f32 scalar ) {
    return { scalar, scalar, scalar, scalar };
}
/// Create vector with given components.
inline vec4 v4( f32 x, f32 y, f32 z, f32 w ) {
    return { x, y, z, w };
}

inline vec4 operator+( vec4 lhs, vec4 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
}
inline vec4 operator-( vec4 lhs, vec4 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
}
inline vec4 operator*( vec4 lhs, f32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
}
inline vec4 operator*( f32 lhs, vec4 rhs ) {
    return operator*( rhs, lhs );
}
inline vec4 operator/( vec4 lhs, f32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
}

namespace IVEC4 {
    global const usize COMPONENT_COUNT = 4;
};

inline ivec4 operator+( ivec4 lhs, ivec4 rhs );
inline ivec4 operator-( ivec4 lhs, ivec4 rhs );
inline ivec4 operator*( ivec4 lhs, i32 rhs );
inline ivec4 operator*( i32 lhs, ivec4 rhs );
inline ivec4 operator/( ivec4 lhs, i32 rhs );
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
    i32 c[IVEC4::COMPONENT_COUNT];

    inline i32& operator[](u32 index)       { return c[index]; }
    inline i32  operator[](u32 index) const { return c[index]; }

    inline b32 operator==( ivec4 b ) const {
        return
            this->x == b.x &&
            this->y == b.y &&
            this->z == b.z &&
            this->w == b.w;
    }
    inline b32 operator!=( ivec4 b ) const { return !(*this == b); }

    inline ivec4 operator-() const { return { -this->x, -this->y, -this->z, -this->w }; }
    inline ivec4& operator+=( ivec4 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    inline ivec4& operator-=( ivec4 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    inline ivec4& operator*=( i32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    inline ivec4& operator/=( i32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
namespace IVEC4 {
    /// { 1, 1, 1, 1 }
    global const ivec4 ONE  = { 1, 1, 1, 1 };
    /// { 0, 0, 0, 0 }
    global const ivec4 ZERO = { 0, 0, 0, 0 };
} // namespace IVEC4

/// Pointer to vector value.
inline i32* value_pointer( const ivec4& v ) { return (i32*)&v.x; }
/// Create zero vector.
inline ivec4 iv4() { return {}; }
/// Create vector with all components set to given scalar.
inline ivec4 iv4( i32 scalar ) {
    return { scalar, scalar, scalar, scalar };
}
/// Create vector with given components.
inline ivec4 iv4( i32 x, i32 y, i32 z, i32 w ) {
    return { x, y, z, w };
}

inline ivec4 operator+( ivec4 lhs, ivec4 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
}
inline ivec4 operator-( ivec4 lhs, ivec4 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
}
inline ivec4 operator*( ivec4 lhs, i32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
}
inline ivec4 operator*( i32 lhs, ivec4 rhs ) {
    return operator*( rhs, lhs );
}
inline ivec4 operator/( ivec4 lhs, i32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
}

/// 3D rotation represented as angle and axis
struct angle_axis {
    vec3 axis;
    f32  theta_radians;
};

inline quat operator+( quat lhs, quat rhs );
inline quat operator-( quat lhs, quat rhs );
inline quat operator*( quat lhs, f32 rhs );
inline quat operator/( quat lhs, f32 rhs );
LD_API quat operator*( quat lhs, quat rhs );

namespace QUAT {
    global const usize COMPONENT_COUNT = 4;
};
/// 3D rotation represented as a quaternion
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
    f32 q[QUAT::COMPONENT_COUNT];

    inline f32& operator[](u32 index)       { return q[index]; }
    inline f32  operator[](u32 index) const { return q[index]; }

    inline b32 operator==( quat b ) const {
        quat tmp = *this - b;
        return (
            tmp.x * tmp.x +
            tmp.y * tmp.y +
            tmp.z * tmp.z +
            tmp.w * tmp.w
        ) < F32::EPSILON;
    }
    inline b32 operator!=( quat rhs ) const { return !(*this == rhs); }

    inline quat operator-() const {
        return {
            -this->w, -this->x,
            -this->y, -this->z
        };
    }
    inline quat& operator+=( quat rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    inline quat& operator-=( quat rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    inline quat& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    inline quat& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
namespace QUAT {
    /// { 0.0, 0.0, 0.0, 0.0 }
    global const quat ZERO     = { 0.0f, 0.0f, 0.0f, 0.0f };
    /// { 1.0, 0.0, 0.0, 0.0 }
    global const quat IDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f };
} // namespace QUAT
/// Pointer to quaternion value.
inline f32* value_pointer( const quat& v ) { return (f32*)&v.w; }
/// Create zero quaternion.
inline quat q() { return {}; }
/// Create quaternion with set to given scalars.
inline quat q( f32 w, f32 x, f32 y, f32 z ) {
    return { w, x, y, z };
}

inline quat operator+( quat lhs, quat rhs ) {
    return {
        lhs.w + rhs.w,
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline quat operator-( quat lhs, quat rhs ) {
    return {
        lhs.w - rhs.w,
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}
inline quat operator*( quat lhs, f32 rhs ) {
    return {
        lhs.w * rhs,
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline quat operator*( f32 lhs, quat rhs ) {
    return operator*( rhs, lhs );
}
inline quat operator/( quat lhs, f32 rhs ) {
    return {
        lhs.w / rhs,
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}

namespace MAT2 {
    global const usize CELL_COUNT   = 4;
    global const usize COLUMN_COUNT = 2;
};

inline mat2 operator+( mat2 lhs, mat2 rhs );
inline mat2 operator-( mat2 lhs, mat2 rhs );
inline mat2 operator*( mat2 lhs, f32 rhs );
inline mat2 operator/( mat2 lhs, f32 rhs );
/// column-major 2x2 32-bit float matrix
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
    vec2 v[MAT2::COLUMN_COUNT];
    f32  c[MAT2::CELL_COUNT];

    inline vec2& operator[]( u32 col ) { return v[col]; }
    inline const vec2& operator[](u32 col) const { return v[col]; }

    /// Copy column out to a vec2.
    inline vec2 column( u32 col ) const { return (*this)[col]; }
    /// Copy row out to a vec2.
    inline vec2 row( u32 row )    const {
        return { this->c[row], this->c[row + 2] };
    }

    inline mat2& operator+=( mat2 rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    inline mat2& operator-=( mat2 rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    inline mat2& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    inline mat2& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
namespace MAT2 {
    /// Zero Matrix.
    global const mat2 ZERO = {
        0.0f, 0.0f,
        0.0f, 0.0f
    };
    /// Identity Matrix.
    global const mat2 IDENTITY = {
        1.0f, 0.0f,
        0.0f, 1.0f
    };
} // namespace MAT2

/// Pointer to matrix value.
inline f32* value_pointer( const mat2& m ) { return (f32*)&m.m00; }
/// Create zero matrix.
inline mat2 m2() { return {}; }
/// Create matrix with given values.
inline mat2 m2( f32 m00, f32 m01, f32 m10, f32 m11 ) {
    return { m00, m01, m10, m11 };
}
inline mat2 operator+( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 + rhs.col0;
    result.col1 = lhs.col1 + rhs.col1;
    return result;
}
inline mat2 operator-( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 - rhs.col0;
    result.col1 = lhs.col1 - rhs.col1;
    return result;
}
inline mat2 operator*( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 * rhs;
    result.col1 = lhs.col1 * rhs;
    return result;
}
inline mat2 operator*( f32 lhs, mat2 rhs ) {
    return operator*( rhs, lhs );
}
inline mat2 operator/( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 / rhs;
    result.col1 = lhs.col1 / rhs;
    return result;
}
inline mat2 operator*( mat2 lhs, mat2 rhs ) {
    return {
        (lhs.c[0] * rhs.c[0]) + (lhs.c[2] * rhs.c[1]),
        (lhs.c[1] * rhs.c[0]) + (lhs.c[3] * rhs.c[2]),

        (lhs.c[0] * rhs.c[2]) + (lhs.c[2] * rhs.c[3]),
        (lhs.c[1] * rhs.c[2]) + (lhs.c[3] * rhs.c[3])
    };
}

namespace MAT3 {
    global const usize CELL_COUNT   = 9;
    global const usize COLUMN_COUNT = 3;
};
inline mat3 operator+( const mat3& lhs, const mat3& rhs );
inline mat3 operator-( const mat3& lhs, const mat3& rhs );
inline mat3 operator*( const mat3& lhs, f32 rhs );
inline mat3 operator/( const mat3& lhs, f32 rhs );
/// column-major 3x3 32-bit float matrix
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
    vec3 v[MAT3::COLUMN_COUNT];
    f32  c[MAT3::CELL_COUNT];

    inline vec3&       operator[]( u32 col )       { return v[col]; }
    inline const vec3& operator[]( u32 col ) const { return v[col]; }

    /// Copy column out to vec3.
    inline vec3 column( u32 col ) const { return (*this)[col]; }
    /// Copy row out to vec3.
    inline vec3 row( u32 row ) const {
        return {
            this->c[row],
            this->c[row + 3],
            this->c[row + 6],
        };
    }

    inline mat3& operator+=( const mat3& rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    inline mat3& operator-=( const mat3& rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    inline mat3& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    inline mat3& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
namespace MAT3 {
    /// Zero Matrix.
    global const mat3 ZERO = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    /// Identity Matrix.
    global const mat3 IDENTITY = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
} // namespace MAT3
/// Pointer to matrix value.
inline f32* value_pointer( const mat3& m ) {
    return (f32*)&m.m00;
}
/// Create zero matrix.
inline mat3 m3() { return {}; }
/// Create matrix from given values.
inline mat3 m3(
    f32 m00, f32 m01, f32 m02,
    f32 m10, f32 m11, f32 m12,
    f32 m20, f32 m21, f32 m22
) {
    return {
        m00, m01, m02,
        m10, m11, m12,
        m20, m21, m22
    };
}
inline mat3 operator+( const mat3& lhs, const mat3& rhs ) {
    mat3 result;
    result.col0 = lhs.col0 + rhs.col0;
    result.col1 = lhs.col1 + rhs.col1;
    result.col2 = lhs.col2 + rhs.col2;
    return result;
}
inline mat3 operator-( const mat3& lhs, const mat3& rhs ) {
    mat3 result;
    result.col0 = lhs.col0 - rhs.col0;
    result.col1 = lhs.col1 - rhs.col1;
    result.col2 = lhs.col2 - rhs.col2;
    return result;
}
inline mat3 operator*( const mat3& lhs, f32 rhs ) {
    mat3 result;
    result.col0 = lhs.col0 * rhs;
    result.col1 = lhs.col1 * rhs;
    result.col2 = lhs.col2 * rhs;
    return result;
}
inline mat3 operator*( f32 lhs, const mat3& rhs ) {
    return operator*( rhs, lhs );
}
inline mat3 operator/( const mat3& lhs, f32 rhs ) {
    mat3 result;
    result.col0 = lhs.col0 / rhs;
    result.col1 = lhs.col1 / rhs;
    result.col2 = lhs.col2 / rhs;
    return result;
}
inline mat3 operator*( const mat3& lhs, const mat3& rhs ) {
    // TODO(alicia): SIMD?
    return {
        // column - 0
        ( lhs.c[0] * rhs.c[0] ) +
            ( lhs.c[3] * rhs.c[1] )  +
            ( lhs.c[6] * rhs.c[2] ),
        ( lhs.c[1] * rhs.c[0] ) +
            ( lhs.c[4] * rhs.c[1] )  +
            ( lhs.c[7] * rhs.c[2] ),
        ( lhs.c[2] * rhs.c[0] ) +
            ( lhs.c[5] * rhs.c[1] )  +
            ( lhs.c[8] * rhs.c[2] ),
        // column - 1
        ( lhs.c[0] * rhs.c[3] ) +
            ( lhs.c[3] * rhs.c[4] )  +
            ( lhs.c[6] * rhs.c[5] ),
        ( lhs.c[1] * rhs.c[3] ) +
            ( lhs.c[4] * rhs.c[4] )  +
            ( lhs.c[7] * rhs.c[5] ),
        ( lhs.c[2] * rhs.c[3] ) +
            ( lhs.c[5] * rhs.c[4] )  +
            ( lhs.c[8] * rhs.c[5] ),
        // column - 2
        ( lhs.c[0] * rhs.c[6] ) +
            ( lhs.c[3] * rhs.c[7] )  +
            ( lhs.c[6] * rhs.c[8] ),
        ( lhs.c[1] * rhs.c[6] ) +
            ( lhs.c[4] * rhs.c[7] )  +
            ( lhs.c[7] * rhs.c[8] ),
        ( lhs.c[2] * rhs.c[6] ) +
            ( lhs.c[5] * rhs.c[7] )  +
            ( lhs.c[8] * rhs.c[8] ),
    };
}

namespace MAT4 {
    global const u32 CELL_COUNT   = 16;
    global const u32 COLUMN_COUNT = 4;
};
LD_API mat4 operator+( const mat4& lhs, const mat4& rhs );
LD_API mat4 operator-( const mat4& lhs, const mat4& rhs );
LD_API mat4 operator*( const mat4& lhs, f32 rhs );
LD_API mat4 operator/( const mat4& lhs, f32 rhs );
/// column-major 4x4 32-bit float matrix
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
    vec4 v[MAT4::COLUMN_COUNT];
    f32  c[MAT4::CELL_COUNT];

    inline vec4&       operator[]( u32 col )       { return v[col]; }
    inline const vec4& operator[]( u32 col ) const { return v[col]; }

    /// Copy column out to a vec4.
    inline vec4 column( u32 col ) const { return (*this)[col]; }
    /// Copy row out to a vec4.
    inline vec4 row( u32 row )    const {
        return {
            this->c[row],
            this->c[row + 4],
            this->c[row + 8],
            this->c[row + 12]
        };
    }

    inline mat4& operator+=( const mat4& rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    inline mat4& operator-=( const mat4& rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    inline mat4& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    inline mat4& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
namespace MAT4 {
    /// Zero Matrix.
    global const mat4 ZERO = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };
    /// Identity Matrix.
    global const mat4 IDENTITY = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
} // namespace MAT4
/// Pointer to matrix value.
LD_ALWAYS_INLINE f32* value_pointer( const mat4& m ) {
    return (f32*)&m.m00;
}
/// Create a zero matrix.
LD_ALWAYS_INLINE mat4 m4() { return {}; }
/// Create a matrix from given values.
LD_ALWAYS_INLINE mat4 m4(
    f32 m00, f32 m01, f32 m02, f32 m03,
    f32 m10, f32 m11, f32 m12, f32 m13,
    f32 m20, f32 m21, f32 m22, f32 m23,
    f32 m30, f32 m31, f32 m32, f32 m33
) {
    return {
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    };
}
inline mat4 operator*( f32 lhs, const mat4& rhs ) {
    return operator*( rhs, lhs );
}
mat4 operator*( const mat4& lhs, const mat4& rhs );
inline vec3 operator*( const mat4& lhs, vec3 rhs ) {
    // TODO(alicia): SIMD?
    return {
        ( lhs.c[0] * rhs[0] )     +
            ( lhs.c[4] * rhs[1] ) +
            ( lhs.c[8] * rhs[2] ) +
            lhs.c[12],
        ( lhs.c[1] * rhs[0] )     +
            ( lhs.c[5] * rhs[1] ) +
            ( lhs.c[9] * rhs[2] ) +
            lhs.c[13],
        ( lhs.c[2] * rhs[0] )      +
            ( lhs.c[6]  * rhs[1] ) +
            ( lhs.c[10] * rhs[2] ) +
            lhs.c[14],
    };
}
inline vec4 operator*( const mat4& lhs, vec4 rhs ) {
    // TODO(alicia): SIMD?
    return {
        ( lhs.c[0] * rhs[0] )      +
            ( lhs.c[4]  * rhs[1] ) +
            ( lhs.c[8]  * rhs[2] ) +
            ( lhs.c[12] * rhs[3] ),
        ( lhs.c[1] * rhs[0] )      +
            ( lhs.c[5]  * rhs[1] ) +
            ( lhs.c[9]  * rhs[2] ) +
            ( lhs.c[13] * rhs[3] ),
        ( lhs.c[2] * rhs[0] )      +
            ( lhs.c[6]  * rhs[1] ) +
            ( lhs.c[10] * rhs[2] ) +
            ( lhs.c[14] * rhs[3] ),
        ( lhs.c[3] * rhs[0] )      +
            ( lhs.c[7]  * rhs[1] ) +
            ( lhs.c[11] * rhs[2] ) +
            ( lhs.c[15] * rhs[3] ),
    };
}

#if defined(LD_COMPILER_CLANG) && !defined(_CLANGD)
    #pragma clang diagnostic pop
#endif

#endif // header guard
