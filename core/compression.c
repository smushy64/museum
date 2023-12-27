/**
 * Description:  Compression scheme implementations.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 13, 2023
*/
#include "shared/defines.h"
#include "core/compression.h"

#include "shared/constants.h"
#include "core/memory.h"
#include "core/math.h"

CORE_API usize compression_rle_encode(
    CompressionStreamFN* stream_out, void* target,
    usize buffer_size, void* buffer, usize* opt_out_encode_size
) {
    if( !buffer_size ) {
        return 0;
    }

    u8*   bytes  = buffer;
    usize result = 0;

    u8 literal_buffer[U8_MAX] = {};
    u8 literal_buffer_len     = 0;

    usize encode_size = 0;

    #define rle_encode( count, ptr )\
        stream_out( target, count, ptr );\
        encode_size += count

    for( usize at = 0; at < buffer_size; ) {
        u8 starting_value = bytes[at];
        u8 run = 1;
        while(
            ( run < ( buffer_size - at ) ) &&
            ( run < U8_MAX ) &&
            ( bytes[at + run] == starting_value )
        ) {
            ++run;
        }

        if( at + 1 >= buffer_size || run > 1 || literal_buffer_len >= U8_MAX ) {
            if( run == 1 && !literal_buffer_len ) {
                result += rle_encode( 1, &run );
                result += rle_encode( 1, &starting_value );
                result += rle_encode( 1, &literal_buffer_len );
            } else {
                result += rle_encode( 1, &literal_buffer_len );
                result += rle_encode( literal_buffer_len, literal_buffer );
                literal_buffer_len = 0;

                result += rle_encode( 1, &run );
                result += rle_encode( 1, &starting_value );
            }
            at += run;
        } else {
            literal_buffer[literal_buffer_len++] = starting_value;
            ++at;
        }

    }

    if( opt_out_encode_size ) {
        *opt_out_encode_size = encode_size;
    }

#undef rle_encode
    return result;
}

CORE_API usize compression_rle_decode(
    CompressionStreamFN* stream_out, void* target,
    usize buffer_size, void* buffer, usize* opt_out_decode_size
) {
    usize result = 0;
    u8* bytes    = buffer;

    usize decode_size = 0;

    #define rle_decode( count, ptr )\
        stream_out( target, count, ptr );\
        decode_size += count

    for( usize at = 0; at < buffer_size; ) {
        u8 count = bytes[at++];
        result += rle_decode( count, bytes + at );
        at += count;

        if( at >= buffer_size ) {
            break;
        }

        count = bytes[at++];

        for( u8 i = 0; i < count; ++i ) {
            result += rle_decode( 1, bytes + at );
        }
        if( count ) {
            ++at;
        }
    }

    if( opt_out_decode_size ) {
        *opt_out_decode_size = decode_size;
    }

    return result;
}

CORE_API usize compression_byte_slice_stream(
    void* target, usize count, void* data
) {
    ByteSlice* slice = target;

    if( count == 1 ) {
        if( slice->len + 1 > slice->capacity ) {
            return 1;
        } else {
            slice->buffer[slice->len++] = *(u8*)data;
            return 0;
        }
    }

    usize remaining = slice->capacity - slice->len;
    usize max_copy  = min( remaining, count );

    memory_copy( slice->buffer + slice->len, data, max_copy );
    slice->len += max_copy;

    return count > remaining ? count - remaining : 0;
}

