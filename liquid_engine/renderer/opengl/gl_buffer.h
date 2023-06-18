#if !defined(RENDERER_OPENGL_GL_BUFFER_HPP)
#define RENDERER_OPENGL_GL_BUFFER_HPP
/**
 * Description:  OpenGL Buffers
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 16, 2023
*/
#include "gl_types.h"

enum BufferDataBaseType : u8 {
    BUFFER_DATA_BASE_TYPE_INT8,
    BUFFER_DATA_BASE_TYPE_UINT8,
    BUFFER_DATA_BASE_TYPE_INT16,
    BUFFER_DATA_BASE_TYPE_UINT16,
    BUFFER_DATA_BASE_TYPE_INT32,
    BUFFER_DATA_BASE_TYPE_UINT32,
    BUFFER_DATA_BASE_TYPE_INT64,
    BUFFER_DATA_BASE_TYPE_UINT64,

    BUFFER_DATA_BASE_TYPE_FLOAT32,
    BUFFER_DATA_BASE_TYPE_FLOAT64,

    BUFFER_DATA_BASE_TYPE_COUNT
};
inline u32 buffer_data_base_type_size( BufferDataBaseType base_type ) {
    local u32 sizes[] = {
        sizeof(i8),
        sizeof(u8),
        sizeof(i16),
        sizeof(u16),
        sizeof(i32),
        sizeof(u32),
        sizeof(i64),
        sizeof(u64),
        sizeof(f32),
        sizeof(f64)
    };
    if( base_type >= BUFFER_DATA_BASE_TYPE_COUNT ) {
        return 0;
    }
    return sizes[base_type];
}
inline GLenum buffer_data_base_type_to_glenum(
    BufferDataBaseType base_type
) {
    local GLenum glenums[] = {
        GL_BYTE,
        GL_UNSIGNED_BYTE,
        GL_SHORT,
        GL_UNSIGNED_SHORT,
        GL_INT,
        GL_UNSIGNED_INT,
        GL_INVALID_ENUM,
        GL_INVALID_ENUM,
        GL_FLOAT,
        GL_DOUBLE
    };
    if( base_type >= BUFFER_DATA_BASE_TYPE_COUNT ) {
        return 0;
    }
    return glenums[base_type];
}
inline const char* to_string( BufferDataBaseType base_type ) {
    local const char* strings[] = {
        "Int8",
        "Uint8",
        "Int16",
        "Uint16",
        "Int32",
        "Uint32",
        "Int64",
        "Uint64",
        "Float32",
        "Float64",
    };
    if( base_type >= BUFFER_DATA_BASE_TYPE_COUNT ) {
        return "Unknown";
    }
    return strings[base_type];
}
enum BufferDataStructType : u8 {
    BUFFER_DATA_STRUCT_TYPE_SCALAR,
    BUFFER_DATA_STRUCT_TYPE_VEC2,
    BUFFER_DATA_STRUCT_TYPE_VEC3,
    BUFFER_DATA_STRUCT_TYPE_VEC4,
    BUFFER_DATA_STRUCT_TYPE_MAT2,
    BUFFER_DATA_STRUCT_TYPE_MAT3,
    BUFFER_DATA_STRUCT_TYPE_MAT4,
    BUFFER_DATA_STRUCT_TYPE_MAT2x3,
    BUFFER_DATA_STRUCT_TYPE_MAT3x2,
    BUFFER_DATA_STRUCT_TYPE_MAT3x4,
    BUFFER_DATA_STRUCT_TYPE_MAT4x3,

    BUFFER_DATA_STRUCT_TYPE_COUNT,
};
inline u32 buffer_data_struct_type_count(
    BufferDataStructType struct_type
) {
    local u32 counts[] = {
        1,  // scalar
        2,  // vec2
        3,  // vec3
        4,  // vec4
        4,  // mat2
        9,  // mat3
        16, // mat4
        6,  // mat2x3
        6,  // mat3x2
        12, // mat3x4
        12 // mat4x3
    };
    if( struct_type >= BUFFER_DATA_STRUCT_TYPE_COUNT ) {
        return 0;
    }
    return counts[struct_type];
}
inline const char* to_string( BufferDataStructType struct_type ) {
    local const char* strings[] = {
        "Scalar",
        "Vec2",
        "Vec3",
        "Vec4",
        "Mat2",
        "Mat3",
        "Mat4",
        "Mat2x3",
        "Mat3x2",
        "Mat3x4",
        "Mat4x3",
    };
    if( struct_type >= BUFFER_DATA_STRUCT_TYPE_COUNT ) {
        return "Unknown";
    }
    return strings[struct_type];
}

struct BufferDataType {
    BufferDataBaseType   base_type;
    BufferDataStructType struct_type;
    u16                  array_count;
};
inline u32 buffer_data_type_size( BufferDataType data_layout ) {
    return
        buffer_data_base_type_size(
            data_layout.base_type
        ) *
        buffer_data_struct_type_count(
            data_layout.struct_type
        ) * (data_layout.array_count ? data_layout.array_count : 1);
}
struct BufferMemoryField {
    BufferDataType data_type;
    u16            field_size;
    u32            field_offset;
};
struct BufferMemoryLayout {
    BufferDataType* data_types;
    u16*            field_sizes;
    u32*            field_offsets;
    u32             field_count;
};
struct UniformBlockBuffer {
    GLuint             handle;
    GLsizeiptr         total_size;
    BufferMemoryLayout memory_layout;
    void*              memory_layout_data;
};
b32 gl_uniform_block_buffer_create_std140(
    UniformBlockBuffer* out_buffer,
    GLuint* opt_buffer_handle,
    void*   opt_buffer_data,
    GLenum  storage_flags,
    u32 field_count,
    ...
);
BufferMemoryField gl_uniform_block_buffer_get_field(
    UniformBlockBuffer* block_buffer,
    u32 field_index
);
void gl_uniform_block_buffer_upload_field(
    UniformBlockBuffer* block_buffer,
    u32 field_index,
    void* data
);
void gl_uniform_block_buffer_free(
    UniformBlockBuffer* block_buffer,
    b32 free_gl_handle
);

#endif // header guard