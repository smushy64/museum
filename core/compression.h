#if !defined(LD_CORE_COMPRESSION_H)
#define LD_CORE_COMPRESSION_H
/**
 * Description:  Compression schemes.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 13, 2023
*/
#include "shared/defines.h"

/// Function prototype for writing bytes to a target.
/// Should return number of bytes NOT written to target.
typedef usize CompressionStreamFN( void* target, usize count, void* data );

/// Function for writing bytes to a byte slice.
/// target MUST be a pointer to a ByteSlice struct.
CORE_API usize compression_byte_slice_stream(
    void* target, usize count, void* data );

/// Encode a buffer of data into a compression stream.
/// Returns number of bytes that did not fit into stream target.
CORE_API usize compression_rle_encode(
    CompressionStreamFN* stream_out, void* target,
    usize buffer_size, void* buffer, usize* opt_out_encode_size );
/// Decode a buffer of compressed data into a compression stream.
/// Returns number of bytes that did not fit into stream target.
CORE_API usize compression_rle_decode(
    CompressionStreamFN* stream_out, void* target,
    usize buffer_size, void* buffer, usize* opt_out_decode_size );

// CORE_API usize compression_huffman_encode(
//     CompressionStreamFN* stream_out, void* target,
//     usize buffer_size, void* buffer, usize* opt_out_encode_size );

#endif /* header guard */
