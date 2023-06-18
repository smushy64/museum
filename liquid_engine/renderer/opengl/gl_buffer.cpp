/**
 * Description:  OpenGL Buffer Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 16, 2023
*/
#include "gl_buffer.h"
#include "gl_backend.h"
#include "gl_functions.h"

#include "core/memory.h"

// TODO(alicia): custom std args ?
#include <stdarg.h>

b32 gl_uniform_block_buffer_create_std140(
    UniformBlockBuffer* out_buffer,
    GLuint* opt_buffer_handle,
    void*   opt_buffer_data,
    GLenum  storage_flags,
    u32 field_count,
    ...
) {
    if( !field_count ) {
        GL_LOG_WARN(
            "Attemped to create a uniform block buffer "
            "with no fields!"
        );
        return false;
    }

    usize data_types_size = sizeof(BufferDataType) * field_count;
    usize field_sizes_size = sizeof(u16) * field_count;
    usize field_offsets_size = sizeof(u32) * field_count;

    usize total_memory_layout_size = data_types_size +
        field_sizes_size +
        field_offsets_size;

    u8* memory_layout_data = (u8*)mem_alloc(
        total_memory_layout_size,
        MEMTYPE_RENDERER
    );
    if( !memory_layout_data ) {
        GL_LOG_ERROR(
            "Failed to allocate %llu bytes for uniform block buffer!",
            total_memory_layout_size
        );
        return false;
    }

    BufferDataType* data_types = (BufferDataType*)memory_layout_data;
    u16* field_sizes   = (u16*)&memory_layout_data[data_types_size];
    u32* field_offsets = (u32*)&memory_layout_data[data_types_size + field_sizes_size];

    va_list list;
    va_start( list, field_count );

    // TODO(alicia): support for structs!
    GLsizeiptr total_size = 0;
    for( u32 i = 0; i < field_count; ++i ) {
        BufferDataType data_type = va_arg( list, BufferDataType );
        data_types[i] = data_type;
        if( data_type.array_count ) {
            u32 element_size = buffer_data_type_size( data_type );
            element_size += element_size % sizeof(vec4);

            u32 array_count = data_type.array_count;

            field_sizes[i] = element_size * array_count;

        } else {
            u32 field_size = 0;
            switch( data_type.struct_type ) {
                case BUFFER_DATA_STRUCT_TYPE_SCALAR: {
                    field_size = buffer_data_base_type_size(
                        data_type.base_type
                    );
                } break;
                case BUFFER_DATA_STRUCT_TYPE_VEC2: {
                    field_size = buffer_data_base_type_size(
                        data_type.base_type
                    ) * 2;
                } break;
                case BUFFER_DATA_STRUCT_TYPE_VEC3:
                case BUFFER_DATA_STRUCT_TYPE_VEC4: {
                    field_size = sizeof( vec4 );
                } break;
                case BUFFER_DATA_STRUCT_TYPE_MAT2: {
                    field_size = buffer_data_base_type_size(
                        data_type.base_type
                    ) * 4;
                } break;
                case BUFFER_DATA_STRUCT_TYPE_MAT4x3:
                case BUFFER_DATA_STRUCT_TYPE_MAT3: {
                    field_size = sizeof( vec4 ) * 3;
                } break;
                case BUFFER_DATA_STRUCT_TYPE_MAT3x4:
                case BUFFER_DATA_STRUCT_TYPE_MAT4: {
                    field_size = sizeof( vec4 ) * 4;
                } break;
                case BUFFER_DATA_STRUCT_TYPE_MAT2x3: {
                    field_size = buffer_data_base_type_size(
                        data_type.base_type
                    ) * 4 * 3;
                } break;
                case BUFFER_DATA_STRUCT_TYPE_MAT3x2: {
                    field_size = sizeof( vec4 ) * 2;
                } break;
                default: break;
            }

            field_sizes[i] = field_size;
        }
        field_offsets[i] = total_size;
        total_size += field_sizes[i];
    }

    va_end( list );

    BufferMemoryLayout memory_layout = {};
    memory_layout.data_types    = data_types;
    memory_layout.field_sizes   = field_sizes;
    memory_layout.field_offsets = field_offsets;
    memory_layout.field_count   = field_count;

    UniformBlockBuffer result = {};
    result.total_size    = total_size;
    result.memory_layout = memory_layout;
    result.memory_layout_data = memory_layout_data;

    if( !opt_buffer_handle ) {
        glCreateBuffers( 1, &result.handle );
    } else {
        result.handle = *opt_buffer_handle;
    }

    glNamedBufferStorage(
        result.handle,
        result.total_size,
        opt_buffer_data,
        storage_flags
    );

    *out_buffer = result;
    return true;
}
BufferMemoryField gl_uniform_block_buffer_get_field(
    UniformBlockBuffer* block_buffer,
    u32 field_index
) {
    BufferMemoryField result = {};
    result.data_type    = block_buffer->memory_layout.data_types[field_index];
    result.field_size   = block_buffer->memory_layout.field_sizes[field_index];
    result.field_offset = block_buffer->memory_layout.field_offsets[field_index];
    return result;
}
void gl_uniform_block_buffer_upload_field(
    UniformBlockBuffer* block_buffer,
    u32 field_index,
    void* data
) {
    BufferMemoryField field = gl_uniform_block_buffer_get_field(
        block_buffer,
        field_index
    );
    glNamedBufferSubData(
        block_buffer->handle,
        field.field_offset,
        field.field_size,
        data
    );
}
void gl_uniform_block_buffer_free(
    UniformBlockBuffer* block_buffer,
    b32 free_gl_handle
) {
    mem_free( block_buffer->memory_layout_data );

    if( free_gl_handle ) {
        glDeleteBuffers( 1, &block_buffer->handle );
    }

    *block_buffer = {};
}
