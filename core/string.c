/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "shared/defines.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/fmt.h"
#include "core/collections.h"

#if defined(LD_ARCH_X86) && LD_SIMD_WIDTH != 1
    #include <immintrin.h>
#endif

CORE_API u64 cstr_hash( usize opt_len, const char* str ) {
    // NOTE(alicia): elf-hash implementation
    // may change in the future!

    usize len = opt_len;
    if( !len ) {
        len = cstr_len( str );
    }

    unsigned char* ustr = (unsigned char*)str;

    u64 x;
    u64 result = x = 0;
    for( usize i = 0; i < len; ++i ) {
        result = ( result << 4 ) + ustr[i];
        x = result & 0xF000000000000000;
        if( x ) {
            result ^= x >> 24;
        }
        result &= ~x;
    }

    return result;
}
CORE_API usize cstr_len( const char* cstr ) {
    if( !cstr ) {
        return 0;
    }
    const char* start = cstr;
    while( *cstr ) {
        cstr++;
    }
    return cstr - start;
}
CORE_API b32 cstr_cmp( const char* a, const char* b ) {
    if( !a || !b ) {
        return false;
    }

    while( *a && *b ) {
        if( *a++ != *b++ ) {
            return false;
        }
        if( (*a && !*b) || (!*a && *b) ) {
            return false;
        }
    }

    return true;
}
CORE_API void cstr_copy(
    char* restricted dst, const char* restricted src, usize opt_src_len
) {
    usize src_len = opt_src_len;
    if( !src_len ) {
        src_len = cstr_len( src );
    }
    memory_copy( dst, src, src_len );
}
CORE_API void cstr_copy_overlapped(
    char* dst, const char* src, usize opt_src_len
) {
    usize src_len = opt_src_len;
    if( !src_len ) {
        src_len = cstr_len( src );
    }
    memory_copy_overlapped( dst, src, src_len );
}

CORE_API void string_slice_iterator( StringSlice slice, Iterator* out_iter ) {
    Iterator result = {0};
    result.buffer    = slice.c;
    result.current   = 0;
    result.count     = slice.len;
    result.item_size = sizeof(char);

    *out_iter = result;
}
CORE_API b32 string_slice_cmp( StringSlice a, StringSlice b ) {
    if( a.len != b.len ) {
        return false;
    }
    return memory_cmp( a.c, b.c, a.len );
}
CORE_API b32 string_slice_find(
    StringSlice slice, StringSlice phrase, usize* opt_out_index
) {
    if( slice.len < phrase.len ) {
        return false;
    }
    
    for( usize i = 0; i < slice.len; ++i ) {
        usize remaining_len = slice.len - i;
        if( remaining_len < phrase.len ) {
            return false;
        }
        StringSlice remaining = string_slice_clip( slice, i, i + phrase.len );

        if( string_slice_cmp( remaining, phrase ) ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }
            return true;
        }
    }

    return false;
}
CORE_API b32 string_slice_find_count(
    StringSlice slice, StringSlice phrase,
    usize* opt_out_first_index, usize* out_count
) {
    usize count = 0;
    if( phrase.len > slice.len ) {
        *out_count = count;
        return false;
    }

    b32   found       = false;
    usize first_index = 0;
    for( usize i = 0; i < slice.len; ++i ) {
        usize remaining_len = slice.len - i;
        if( phrase.len > remaining_len ) {
            break;
        }

        StringSlice current = string_slice_clip( slice, i, phrase.len );

        if( string_slice_cmp( current, phrase ) ) {
            if( !found ) {
                first_index = i;
            }
            found = true;
            count++;
        }
    }

    if( found && opt_out_first_index ) {
        *opt_out_first_index = first_index;
    }

    *out_count = count;
    return found;
}
CORE_API b32 string_slice_find_char(
    StringSlice slice, char character, usize* opt_out_index
) {
    if( !slice.len ) {
        return false;
    }

#if defined(LD_ARCH_X86) && LD_SIMD_WIDTH != 1
    char* aligned_ptr = memory_align( slice.c, 16 );

    usize aligned_remaining_len = aligned_ptr - slice.c;
    if( aligned_remaining_len > slice.len ) {
        goto find_by_char;
    }

    // check up to aligned pointer
    usize result = 0;
    char* at = slice.c;
    while( at != aligned_ptr ) {
        if( *at++ == character ) {
            if( opt_out_index ) {
                *opt_out_index = result;
            }
            return true;
        }
        result++;
    }

    // check using simd
    __m128i* current   = (__m128i*)aligned_ptr;
    __m128i  wide_char = _mm_set1_epi8( character );
    loop {
        __m128i loaded_string = _mm_load_si128( current );
        __m128i cmp_result    = _mm_cmpeq_epi8( loaded_string, wide_char );

        u16 mask = _mm_movemask_epi8( cmp_result );

        if( mask ) {
            u16 offset = 0;
            // if match was found in second half of chunk
            // this is just to skip checking first half of chunk
            if( !(mask & (0xFF)) && (mask & (0xFF << 8)) ) {
                result += 8;
                offset  = 8;
            }

            for( u16 i = 0; i < 8; ++i ) {
                if( ( mask >> (i + offset) ) & 0x1 ) {
                    break;
                }
                result++;
            }

            if( result >= slice.len ) {
                return false;
            }

            if( opt_out_index ) {
                *opt_out_index = result;
            }
            return true;
        }

        current++;
        result += 16;
        if( result >= slice.len ) {
            return false;
        }
    }

#endif /* SIMD x86 */

find_by_char:
    for( usize i = 0; i < slice.len; ++i ) {
        if( slice.c[i] == character ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }
            return true;
        }
    }
    return false;
}
CORE_API b32 string_slice_find_char_count(
    StringSlice slice, char character,
    usize* opt_out_first_index, usize* out_count
) {
    StringSlice sub_slice = slice;
    usize count = 0;
    loop {
        usize index = 0;
        if( string_slice_find_char( sub_slice, character, &index ) ) {
            if( !count && opt_out_first_index ) {
                *opt_out_first_index = index;
            }
            count++;

            if( index + 1 > sub_slice.len ) {
                *out_count = count;
                return count != 0;
            }

            sub_slice.c += index + 1;
            sub_slice.len    -= index + 1;
        } else {
            *out_count = count;
            return count != 0;
        }
    }
}
CORE_API b32 string_slice_find_whitespace( StringSlice slice, usize* opt_out_index ) {
    for( usize i = 0; i < slice.len; ++i ) {
        if( char_is_whitespace( slice.c[i] ) ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }

            return true;
        }
    }

    return false;
}
CORE_API void string_slice_reverse( StringSlice slice ) {
    char* begin = slice.c;
    char* end   = begin + slice.len - 1;

    while( begin != end ) {
        char temp = *end;
        *end--   = *begin;
        b32 should_end = begin == end;
        *begin++ = temp;

        if( should_end ) {
            return;
        }
    }
}
CORE_API StringSlice string_slice_trim_leading_whitespace(
    StringSlice slice
) {
    StringSlice result = slice;
    for( usize i = 0; i < result.len; ++i ) {
        char current = result.c[i];
        if( char_is_whitespace( current ) ) {
            continue;
        }

        result.c += i;
        result.len    -= i;
        break;
    }

    return result;
}
CORE_API StringSlice string_slice_trim_trailing_whitespace(
    StringSlice slice
) {
    for( usize i = slice.len; i-- > 0; ) {
        char current = slice.c[i];
        if( !char_is_whitespace( current ) ) {
            slice.len = i + 1;
            break;
        }
    }
    return slice;
}
CORE_API void string_slice_set( StringSlice slice, char character ) {
    memory_set( slice.c, character, slice.len );
}
CORE_API void string_slice_to_upper( StringSlice slice ) {
    for( usize i = 0; i < slice.len; ++i ) {
        char* current = slice.c + i;
        if( *current >= 'a' && *current <= 'z' ) {
            *current -= 'a' - 'A';
        }
    }
}
CORE_API void string_slice_to_lower( StringSlice slice ) {
    for( usize i = 0; i < slice.len; ++i ) {
        char* current = slice.c + i;
        if( *current >= 'A' && *current <= 'Z' ) {
            *current += 'a' - 'A';
        }
    }
}
CORE_API StringSlice string_slice_clip(
    StringSlice slice, usize from_inclusive, usize to_exclusive
) {
    assert( from_inclusive < slice.len );
    assert( to_exclusive <= slice.len );

    StringSlice result = {};

    result.c = slice.c + from_inclusive;
    result.len    = to_exclusive - from_inclusive;

    return result;
}
CORE_API b32 string_slice_pop(
    StringSlice slice, StringSlice* out_slice, char* opt_out_character
) {
    if( !slice.len ) {
        return false;
    }

    out_slice->c = slice.c;
    out_slice->len    = slice.len - 1;

    if( opt_out_character ) {
        *opt_out_character = slice.c[slice.len];
    }

    return true;
}
CORE_API b32 string_slice_pop_start(
    StringSlice slice, StringSlice* out_slice, char* opt_out_character
) {
    if( !slice.len ) {
        return false;
    }

    if( opt_out_character ) {
        *opt_out_character = slice.c[0];
    }

    out_slice->c = slice.c + 1;
    out_slice->len    = slice.len - 1;

    return true;
}
CORE_API void string_slice_split(
    StringSlice slice, usize index,
    StringSlice* opt_out_first, StringSlice* opt_out_last
) {
    assert( index < slice.len );

    if( opt_out_first ) {
        opt_out_first->c = slice.c;
        opt_out_first->len    = index;
    }

    if( opt_out_last && index + 1 < slice.len ) {
        opt_out_last->c = slice.c + index + 1;
        opt_out_last->len    = slice.len - index;
        if( opt_out_last->len ) {
            opt_out_last->len -= 1;
        }
    }
}
CORE_API b32 string_slice_split_char(
    StringSlice slice, char character,
    StringSlice* out_first, StringSlice* out_last
) {
    usize index = 0;
    if( !string_slice_find_char( slice, character, &index ) ) {
        return false;
    }

    string_slice_split( slice, index, out_first, out_last );
    return true;
}
CORE_API b32 string_slice_split_whitespace(
    StringSlice slice, StringSlice* opt_out_first, StringSlice* opt_out_last
) {
    usize index = 0;
    if( !string_slice_find_whitespace( slice, &index ) ) {
        return false;
    }

    string_slice_split( slice, index, opt_out_first, opt_out_last );

    if( opt_out_last ) {
        *opt_out_last = string_slice_trim_leading_whitespace( *opt_out_last );
    }

    return true;
}
internal b32 ___safe_increment( usize* val, usize max ) {
    if( *val + 1 >= max ) {
        return false;
    } else {
        *val += 1;
        return true;
    }
}
CORE_API b32 string_slice_parse_int( StringSlice slice, i64* out_integer ) {
    if( !slice.len ) {
        return false;
    }

    b32 is_negative = false;
    i64 result = 0;

    usize at = 0;
    StringSlice parse = slice;
    if( parse.c[at] == '-' ) {
        if( !___safe_increment( &at, parse.len ) ) {
            return false;
        }
        is_negative = true;
    }

    do {
        if( !char_is_digit( parse.c[at] ) ) {
            if( !at ) {
                return false;
            }
            break;
        }

        result *= 10;
        result += (parse.c[at] - '0');
    } while( ___safe_increment( &at, parse.len ) );

    *out_integer = result * ( is_negative ? -1 : 1 );
    return true;
}
CORE_API b32 string_slice_parse_uint( StringSlice slice, u64* out_integer ) {
    if( !slice.len ) {
        return false;
    }

    u64 result = 0;

    usize at = 0;
    StringSlice parse = slice;

    do {
        if( !char_is_digit( parse.c[at] ) ) {
            if( !at ) {
                return false;
            }
            break;
        }

        result *= 10;
        result += (parse.c[at] - '0');
    } while( ___safe_increment( &at, parse.len ) );

    *out_integer = result;
    return true;
}
internal u64 ___places( u64 i ) {
    if( i < 10 ) {
        return 1;
    } else if( i < 100 ) {
        return 2;
    } else if( i < 1000 ) {
        return 3;
    } else if( i < 10000 ) {
        return 4;
    } else if( i < 100000 ) {
        return 5;
    } else if( i < 1000000 ) {
        return 6;
    } else if( i < 10000000 ) {
        return 7;
    } else if( i < 100000000 ) {
        return 8;
    } else if( i < 1000000000 ) {
        return 9;
    } else if( i < 10000000000 ) {
        return 10;
    } else if( i < 100000000000 ) {
        return 11;
    } else if( i < 1000000000000 ) {
        return 12;
    } else if( i < 10000000000000 ) {
        return 13;
    } else if( i < 100000000000000 ) {
        return 14;
    } else if( i < 1000000000000000 ) {
        return 15;
    } else if( i < 10000000000000000 ) {
        return 16;
    } else if( i < 100000000000000000 ) {
        return 17;
    } else if( i < 1000000000000000000 ) {
        return 18;
    }

    return 0;
}
internal f32 ___internal_poweri( f32 base, i32 exp ) {
    u32 exp_abs = exp < 0 ? (u32)(-exp) : (u32)(exp);
    f32 result       = base;
    for( u32 i = 1; i < exp_abs; ++i ) {
        result *= base;
    }
    if( exp < 0 ) {
        return 1.0f / result;
    } else {
        return result;
    }
}
CORE_API b32 string_slice_parse_float( StringSlice slice, f64* out_float ) {
    // f64 result = 0.0;
    i64 whole_part      = 0;
    u64 fractional_part = 0;

    usize dot_position = 0;
    if(
        string_slice_find_char( slice, '.', &dot_position ) &&
        dot_position + 1 < slice.len
    ) {
        StringSlice first = {0}, last = {0};
        string_slice_split( slice, dot_position, &first, &last );
        if( !string_slice_parse_int( first, &whole_part ) ) {
            return false;
        }

        usize zero_count = 0;
        for( usize i = 0; i < last.len; ++i ) {
            if( last.c[i] == '0' ) {
                zero_count++;
            }
        }

        last.c += zero_count;
        last.len    -= zero_count;

        if( last.len ) {
            if( !string_slice_parse_uint( last, &fractional_part ) ) {
                return false;
            }
        }

        *out_float = (f64)whole_part;
        f64 fractional_part_f64 = (f64)fractional_part;
        u64 places = ___places( fractional_part );

        u64 pow = places + zero_count;
        if( pow ) {
            fractional_part_f64 /= (f64)___internal_poweri( 10.0f, pow );
        }

        *out_float += fractional_part_f64;

        return true;
    } else {
        b32 success = string_slice_parse_int( slice, &whole_part );
        if( success ) {
            *out_float = (f64)whole_part;
            return success;
        } else {
            return success;
        }
    }
}
CORE_API usize string_buffer_copy( StringBuffer* dst, StringSlice src ) {
    usize available_space = dst->cap - dst->len;
    usize max_copy = src.len;
    if( max_copy > available_space ) {
        max_copy = available_space;
    }

    memory_copy( dst->c + dst->len, src.c, max_copy );
    dst->len += max_copy;

    return src.len - max_copy;
}
CORE_API b32 string_buffer_prepend( StringBuffer* dst, StringSlice src ) {
    usize final_len = dst->len + src.len;
    if( final_len > dst->cap ) {
        return false;
    }

    memory_copy_overlapped( dst->c + src.len, dst->c, dst->len );
    memory_copy( dst->c, src.c, src.len );
    dst->len = final_len;

    return true;
}
CORE_API b32 string_buffer_append( StringBuffer* dst, StringSlice src ) {
    usize final_len = dst->len + src.len;
    if( final_len > dst->cap ) {
        return false;
    }

    memory_copy( dst->c + dst->len, src.c, src.len );
    dst->len = final_len;

    return true;
}
CORE_API void string_buffer_fill( StringBuffer* buffer, char character ) {
    memory_set( buffer->c, character, buffer->cap );
    buffer->len = buffer->cap;
}
CORE_API b32 string_buffer_push( StringBuffer* buffer, char character ) {
    if( buffer->len == buffer->cap ) {
        return false;
    }

    buffer->c[buffer->len++] = character;

    return true;
}
CORE_API b32 string_buffer_pop( StringBuffer* buffer, char* opt_out_character ) {
    if( !buffer->len ) {
        return false;
    }

    char character = buffer->c[--buffer->len];
    if( opt_out_character ) {
        *opt_out_character = character;
    }

    return true;
}
CORE_API void string_buffer_remove(
    StringBuffer* buffer, usize index, char* opt_out_character
) {
    assert( index < buffer->len );
    if( index == buffer->len - 1 ) {
        string_buffer_pop( buffer, opt_out_character );
        return;
    }

    if( opt_out_character ) {
        *opt_out_character = buffer->c[index];
    }

    memory_copy_overlapped(
        buffer->c + index,
        buffer->c + index + 1,
        buffer->len - index );

    buffer->len--;
}
CORE_API b32 string_buffer_insert(
    StringBuffer* buffer, usize index, char character
) {
    if( index == buffer->len - 1 ) {
        return string_buffer_push( buffer, character );
    }
    if( buffer->len == buffer->cap ) {
        return false;
    }

    usize copy_size = buffer->len - index;
    memory_copy_overlapped(
        buffer->c + index + 1,
        buffer->c + index,
        copy_size );

    buffer->c[index] = character;
    buffer->len++;

    return true;
}
CORE_API b32 string_buffer_insert_phrase(
    StringBuffer* buffer, usize index, StringSlice phrase
) {
    if( index == buffer->len - 1 ) {
        return string_buffer_append( buffer, phrase );
    }

    usize new_len = buffer->len + phrase.len;
    if( new_len > buffer->cap ) {
        return false;
    }

    usize copy_size = buffer->len - index;
    memory_copy_overlapped(
        buffer->c + index + phrase.len,
        buffer->c + index,
        copy_size );
    memory_copy( buffer->c + index, phrase.c, phrase.len );
    buffer->len += phrase.len;

    return true;
}
CORE_API usize string_buffer_write( void* target, usize count, char* characters ) {
    if( !count ) {
        return 0;
    }

    StringBuffer* buffer = target;
    if( count == 1 ) {
        return !string_buffer_push( buffer, *characters );
    }

    usize max_copy = count;
    if( buffer->len + max_copy > buffer->cap ) {
        max_copy = buffer->cap - buffer->len;
    }

    memory_copy( buffer->c + buffer->len, characters, max_copy );
    buffer->len += max_copy;

    return count - max_copy;
}
CORE_API usize string_buffer_fmt_cstr_va(
    StringBuffer* buffer, usize format_len, const char* format, va_list va
) {
    usize result = fmt_write_va( string_buffer_write, buffer, format_len, format, va );
    return result;
}
CORE_API usize string_buffer_fmt_bool( StringBuffer* slice, b32 b, b32 binary ) {
    return fmt_write_bool( string_buffer_write, slice, b, binary );
}
CORE_API usize string_buffer_fmt_float( StringBuffer* slice, f64 f, u32 precision ) {
    return fmt_write_float( string_buffer_write, slice, f, precision );
}
CORE_API usize string_buffer_fmt_i8( StringBuffer* slice, i8 i, enum FormatInteger format ) {
    return fmt_write_i8( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_u8( StringBuffer* slice, u8 i, enum FormatInteger format ) {
    return fmt_write_u8( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_i16( StringBuffer* slice, i16 i, enum FormatInteger format ) {
    return fmt_write_i16( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_u16( StringBuffer* slice, u16 i, enum FormatInteger format ) {
    return fmt_write_u16( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_i32( StringBuffer* slice, i32 i, enum FormatInteger format ) {
    return fmt_write_i32( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_u32( StringBuffer* slice, u32 i, enum FormatInteger format ) {
    return fmt_write_u32( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_i64( StringBuffer* slice, i64 i, enum FormatInteger format ) {
    return fmt_write_i64( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_u64( StringBuffer* slice, u64 i, enum FormatInteger format ) {
    return fmt_write_u64( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_isize(
    StringBuffer* slice, isize i, enum FormatInteger format
) {
    return fmt_write_isize( string_buffer_write, slice, i, format );
}
CORE_API usize string_buffer_fmt_usize(
    StringBuffer* slice, usize i, enum FormatInteger format
) {
    return fmt_write_usize( string_buffer_write, slice, i, format );
}

