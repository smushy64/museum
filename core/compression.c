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

#include "core/sort.h"

// struct MinHeapNode {
//     char c;
//     usize frequency;
//
//     struct MinHeapNode* left, *right;
// };
// b32 min_heap_node_lt( void* lhs, void* rhs, void* in_params ) {
//     unused( in_params );
//     struct MinHeapNode* l = lhs;
//     struct MinHeapNode* r = rhs;
//
//     return l->frequency < r->frequency;
// }
// void min_heap_node_sort_swap( void* lhs, void* rhs ) {
//     struct MinHeapNode* l = lhs;
//     struct MinHeapNode* r = rhs;
//     struct MinHeapNode temp = *l;
//     *l = *r;
//     *r = temp;
// }
//
// struct MinHeap {
//     usize size;
//     usize count;
//     struct MinHeapNode** array;
// };
//
// void min_heap_node_swap( struct MinHeapNode** a, struct MinHeapNode** b ) {
//     struct MinHeapNode* temp = *a;
//     *a = *b;
//     *b = temp;
// }
//
// void min_heap_heapify( struct MinHeap* min_heap, usize index ) {
//     usize smallest = index;
//     usize left     = 2 * index + 1;
//     usize right    = 2 * index + 2;
//
//     if(
//         left < min_heap->size &&
//         min_heap->array[left]->frequency < min_heap->array[smallest]->frequency
//     ) {
//         smallest = left;
//     }
//
//     if(
//         right < min_heap->size &&
//         min_heap->array[right]->frequency < min_heap->array[smallest]->frequency
//     ) {
//         smallest = right;
//     }
//
//     if( smallest != index ) {
//         min_heap_node_swap( &min_heap->array[smallest], &min_heap->array[index] );
//         min_heap_heapify( min_heap, smallest );
//     }
// }
//
// b32 min_heap_is_one( struct MinHeap* min_heap ) {
//     return min_heap->size == 1;
// }
//
// CORE_API usize compression_huffman_encode(
//     CompressionStreamFN* stream_out, void* target,
//     usize buffer_size, void* in_buffer, usize* opt_out_encode_size
// ) {
//     struct MinHeapNode node_buffer[256] = {};
//
//     unsigned char* buffer = in_buffer;
//
//     usize node_count = 0;
//     for( usize i = 0; i < buffer_size; ++i ) {
//         usize index = buffer[i];
//         node_buffer[index].c = buffer[i];
//
//         if( !node_buffer[index].frequency ) {
//             node_count++;
//         }
//         node_buffer[index].frequency++;
//     }
//
//     sorting_quicksort(
//         0, 255, sizeof( node_buffer[0] ), node_buffer,
//         min_heap_node_lt, NULL, min_heap_node_sort_swap );
//
//     struct MinHeapNode* nodes =
//         (node_buffer + static_array_count( node_buffer )) - node_count;
//
//     unused(nodes, target, stream_out, opt_out_encode_size);
//     return 0;
// }
//
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

