/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/fmt.h"
#include "core/internal.h"

LD_API Iterator string_slice_iterator( StringSlice* slice ) {
    Iterator result = {0};
    result.buffer    = slice->buffer;
    result.current   = 0;
    result.count     = slice->len;
    result.item_size = sizeof(char);

    return result;
}

LD_API usize cstr_len( const char* cstr ) {
    if( !cstr ) {
        return 0;
    }
    const char* start = cstr;
    while( *cstr ) {
        cstr++;
    }
    return cstr - start;
}
LD_API b32 cstr_cmp( const char* a, const char* b ) {
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
LD_API void cstr_copy(
    char* restricted dst, const char* restricted src, usize opt_src_len
) {
    usize src_len = opt_src_len;
    if( !src_len ) {
        src_len = cstr_len( src );
    }
    memory_copy( dst, src, src_len );
}
LD_API void cstr_copy_overlapped(
    char* dst, const char* src, usize opt_src_len
) {
    usize src_len = opt_src_len;
    if( !src_len ) {
        src_len = cstr_len( src );
    }
    memory_copy_overlapped( dst, src, src_len );
}

hot LD_API void string_slice_output_stdout( StringSlice* slice ) {
    platform->io.console_write(
        platform->io.stdout_handle(),
        slice->len, slice->buffer
    );
}
hot LD_API void string_slice_output_stderr( StringSlice* slice ) {
    platform->io.console_write(
        platform->io.stderr_handle(),
        slice->len, slice->buffer
    );
}
internal b32 ___safe_increment( usize* val, usize max ) {
    if( *val + 1 >= max ) {
        return false;
    } else {
        *val += 1;
        return true;
    }
}
LD_API b32 string_slice_parse_int( StringSlice* slice, i64* out_integer ) {
    if( !slice->len ) {
        return false;
    }

    b32 is_negative = false;
    i64 result = 0;

    usize at = 0;
    StringSlice parse = string_slice_clone( slice );
    if( parse.buffer[at] == '-' ) {
        if( !___safe_increment( &at, parse.len ) ) {
            return false;
        }
        is_negative = true;
    }

    do {
        if( !char_is_digit( parse.buffer[at] ) ) {
            if( !at ) {
                return false;
            }
            break;
        }

        result *= 10;
        result += (parse.buffer[at] - '0');
    } while( ___safe_increment( &at, parse.len ) );

    *out_integer = result * ( is_negative ? -1 : 1 );
    return true;
}
LD_API b32 string_slice_parse_uint( StringSlice* slice, u64* out_integer ) {
    if( !slice->len ) {
        return false;
    }

    u64 result = 0;

    usize at = 0;
    StringSlice parse = string_slice_clone( slice );

    do {
        if( !char_is_digit( parse.buffer[at] ) ) {
            if( !at ) {
                return false;
            }
            break;
        }

        result *= 10;
        result += (parse.buffer[at] - '0');
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

LD_API b32 string_slice_parse_float( StringSlice* slice, f64* out_float ) {
    // f64 result = 0.0;
    i64 whole_part      = 0;
    u64 fractional_part = 0;

    usize dot_position = 0;
    if(
        string_slice_find_char( slice, '.', &dot_position ) &&
        dot_position + 1 < slice->len
    ) {
        StringSlice first = {0}, last = {0};
        string_slice_split( slice, dot_position, &first, &last );
        if( !string_slice_parse_int( &first, &whole_part ) ) {
            return false;
        }

        usize zero_count = 0;
        for( usize i = 0; i < last.len; ++i ) {
            if( last.buffer[i] == '0' ) {
                zero_count++;
            }
        }

        last.buffer += zero_count;
        last.len    -= zero_count;

        if( last.len ) {
            if( !string_slice_parse_uint( &last, &fractional_part ) ) {
                return false;
            }
        }

        *out_float = (f64)whole_part;
        f64 fractional_part_f64 = (f64)fractional_part;
        u64 places = ___places( fractional_part );

        u64 pow = places + zero_count;
        if( pow ) {
            fractional_part_f64 /= (f64)poweri( 10.0f, pow );
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
LD_API StringSlice string_slice_from_cstr( usize opt_len, const char* cstr ) {
    StringSlice result;
    result.buffer = (char*)cstr;
    if( opt_len ) {
        result.len = opt_len;
    } else {
        result.len = cstr_len( cstr );
    }
    result.capacity = result.len + 1;
    return result;
}
LD_API b32 string_slice_cmp( StringSlice* a, StringSlice* b ) {
    if( a->len != b->len ) {
        return false;
    }
    for( usize i = 0; i < a->len; ++i ) {
        if( a->buffer[i] != b->buffer[i] ) {
            return false;
        }
    }

    return true;
}
LD_API b32 string_slice_find(
    StringSlice* slice, StringSlice* phrase, usize* opt_out_index
) {
    if( slice->len < phrase->len ) {
        return false;
    }
    
    for( usize i = 0; i < slice->len; ++i ) {
        usize remaining_len = slice->len - i;
        if( remaining_len < phrase->len ) {
            return false;
        }
        StringSlice remaining;
        remaining.buffer   = slice->buffer + i;
        remaining.len      = phrase->len;
        remaining.capacity = remaining.len;

        if( string_slice_cmp( &remaining, phrase ) ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }
            return true;
        }
    }

    return false;

}
LD_API void string_slice_to_upper( StringSlice* slice ) {
    for( usize i = 0; i < slice->len; ++i ) {
        char* current = slice->buffer + i;
        if( *current >= 'a' && *current <= 'z' ) {
            *current -= 'a' - 'A';
        }
    }
}
LD_API void string_slice_to_lower( StringSlice* slice ) {
    for( usize i = 0; i < slice->len; ++i ) {
        char* current = slice->buffer + i;
        if( *current >= 'A' && *current <= 'Z' ) {
            *current += 'a' - 'A';
        }
    }
}
LD_API b32 string_slice_find_count(
    StringSlice* slice, StringSlice* phrase,
    usize* opt_out_first_index, usize* out_count
) {
    usize count = 0;
    if( phrase->len > slice->len ) {
        *out_count = count;
        return false;
    }

    b32   found       = false;
    usize first_index = 0;
    for( usize i = 0; i < slice->len; ++i ) {
        usize remaining_len = slice->len - i;
        if( phrase->len > remaining_len ) {
            break;
        }

        StringSlice current;
        current.buffer = slice->buffer + i;
        current.len    = phrase->len;

        if( string_slice_cmp( &current, phrase ) ) {
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
LD_API b32 string_slice_find_char(
    StringSlice* slice, char character, usize* opt_out_index
) {
    for( usize i = 0; i < slice->len; ++i ) {
        if( slice->buffer[i] == character ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }
            return true;
        }
    }
    return false;
}
LD_API b32 string_slice_find_char_count(
    StringSlice* slice, char character,
    usize* opt_out_first_index, usize* out_count
) {
    b32   found       = false;
    usize first_index = 0;
    usize count       = 0;

    for( usize i = 0; i < slice->len; ++i ) {
        if( slice->buffer[i] == character ) {
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
LD_API b32 string_slice_find_whitespace( StringSlice* slice, usize* opt_out_index ) {
    for( usize i = 0; i < slice->len; ++i ) {
        if( char_is_whitespace( slice->buffer[i] ) ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }

            return true;
        }
    }

    return false;
}
LD_API void string_slice_copy_to_len( StringSlice* dst, StringSlice* src ) {
    usize max_copy = dst->len;
    if( max_copy > src->len ) {
        max_copy = src->len;
    }
    memory_copy( dst->buffer, src->buffer, max_copy );
}
LD_API void string_slice_copy_to_capacity( StringSlice* dst, StringSlice* src ) {
    usize max_copy = dst->capacity;
    if( max_copy > src->len ) {
        max_copy = src->len;
    }
    memory_copy( dst->buffer, src->buffer, max_copy );
    if( max_copy > dst->len ) {
        dst->len = max_copy;
    }
}
LD_API void string_slice_reverse( StringSlice* slice ) {
    char* begin = slice->buffer;
    char* end   = begin + slice->len - 1;

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
LD_API void string_slice_trim_leading_whitespace(
    StringSlice* slice, StringSlice* out_trimmed
) {
    StringSlice result = string_slice_clone( slice );
    for( usize i = 0; i < result.len; ++i ) {
        char current = result.buffer[i];
        if( char_is_whitespace( current ) ) {
            continue;
        }

        result.buffer += i;
        result.len    -= i;
        break;
    }

    *out_trimmed = result;
}
LD_API void string_slice_trim_trailing_whitespace(
    StringSlice* slice, StringSlice* out_trimmed
) {
    StringSlice result = string_slice_clone( slice );
    for( usize i = result.len; i-- > 0; ) {
        char current = result.buffer[i];
        if( char_is_whitespace( current ) ) {
            result.len = i;
            break;
        }
    }
    *out_trimmed = result;
}
LD_API void string_slice_fill_to_len( StringSlice* slice, char character ) {
    memory_set( slice->buffer, *(u8*)&character, slice->len );
}
LD_API void string_slice_fill_to_capacity( StringSlice* slice, char character ) {
    slice->len = slice->capacity;
    memory_set( slice->buffer, *(u8*)&character, slice->len );
}
LD_API b32 string_slice_clip(
    StringSlice* src,
    usize from_inclusive, usize to_exclusive,
    StringSlice* out_dst
) {
    if(
        from_inclusive > to_exclusive ||
        from_inclusive >= src->len ||
        to_exclusive > src->len
    ) {
        return false;
    }

    StringSlice result = {};
    result.buffer   = src->buffer + from_inclusive;
    result.len      = to_exclusive - from_inclusive;
    result.capacity = src->capacity - from_inclusive;

    *out_dst = result;

    return true;
}
LD_API b32 string_slice_push( StringSlice* slice, char character ) {
    if( slice->len == slice->capacity ) {
        return false;
    }

    slice->buffer[slice->len++] = character;

    return true;
}
LD_API b32 string_slice_pop( StringSlice* slice, char* opt_out_char ) {
    if( !slice->len ) {
        return false;
    }

    slice->len--;
    char c = slice->buffer[slice->len];
    if( opt_out_char ) {
        *opt_out_char = c;
    }
    
    return true;
}
LD_API b32 string_slice_pop_start( StringSlice* slice, char* opt_out_char ) {
    if( !slice->len ) {
        return false;
    }

    char c = slice->buffer[0];
    if( opt_out_char ) {
        *opt_out_char = c;
    }

    slice->buffer++;
    slice->len--;

    return true;
}
LD_API b32 string_slice_insert_char(
    StringSlice* slice, usize index, char character
) {
    if( index == slice->len ) {
        return string_slice_push( slice, character );
    }
    if( slice->len == slice->capacity || index > slice->len ) {
        return false;
    }

    usize remaining_len = slice->len++ - index;
    memory_copy_overlapped(
        slice->buffer + index + 1, slice->buffer + index, remaining_len );
    slice->buffer[index] = character;

    return true;
}
LD_API b32 string_slice_prepend( StringSlice* slice, StringSlice* prepend ) {
    usize len = slice->len + prepend->len;
    if( len > slice->capacity ) {
        return false;
    }

    memory_copy_overlapped( slice->buffer + prepend->len, slice->buffer, slice->len );
    memory_copy( slice->buffer, prepend->buffer, prepend->len );

    slice->len = len;

    return true;
}
LD_API b32 string_slice_insert(
    StringSlice* slice, usize index, StringSlice* insert
) {
    if( !index ) {
        return string_slice_prepend( slice, insert );
    }
    if( index == slice->len ) {
        return string_slice_append( slice, insert );
    }
    if( index > slice->len ) {
        return false;
    }

    usize required_capacity = slice->len + insert->len;
    if( required_capacity > slice->capacity ) {
        return false;
    }

    usize remaining_len = slice->len - index;
    memory_copy_overlapped(
        slice->buffer + index,
        slice->buffer + index + insert->len, remaining_len );
    memory_copy( slice->buffer + index, insert->buffer, insert->len );

    slice->len = required_capacity;
    return true;
}
LD_API b32 string_slice_append( StringSlice* slice, StringSlice* append ) {
    usize required_capacity = slice->len + append->len;
    if( required_capacity > slice->capacity ) {
        return false;
    }

    memory_copy( slice->buffer + slice->len, append->buffer, append->len );
    slice->len = required_capacity;

    return true;
}
LD_API b32 string_slice_prepend_cstr( StringSlice* slice, const char* prepend ) {
    StringSlice prepend_slice = string_slice_from_cstr( 0, prepend );
    return string_slice_prepend( slice, &prepend_slice );
}
LD_API b32 string_slice_append_cstr( StringSlice* slice, const char* append ) {
    StringSlice append_slice = string_slice_from_cstr( 0, append );
    return string_slice_append( slice, &append_slice );
}
LD_API b32 string_slice_insert_cstr(
    StringSlice* slice, usize index, const char* insert
) {
    StringSlice insert_slice = string_slice_from_cstr( 0, insert );
    return string_slice_insert( slice, index, &insert_slice );
}
LD_API void string_slice_split(
    StringSlice* slice_to_split, usize index,
    StringSlice* out_first, StringSlice* out_last
) {
    assert( index + 1 < slice_to_split->len );

    out_first->buffer   = slice_to_split->buffer;
    out_first->len      = index;
    out_first->capacity = out_first->len;

    usize offset = index + 1;
    out_last->buffer    = slice_to_split->buffer + offset;
    out_last->len       = slice_to_split->len - offset;
    out_last->capacity  = slice_to_split->capacity - offset;
}
LD_API b32 string_slice_split_char(
    StringSlice* slice, char character, StringSlice* out_first, StringSlice* out_last
) {
    usize index = 0;
    if( !string_slice_find_char( slice, character, &index ) ) {
        return false;
    }

    string_slice_split( slice, index, out_first, out_last );

    return true;
}
LD_API b32 string_slice_split_whitespace(
    StringSlice* slice, StringSlice* out_first, StringSlice* out_last
) {
    usize index = 0;
    if( !string_slice_find_whitespace( slice, &index ) ) {
        return false;
    }

    string_slice_split( slice, index, out_first, out_last );

    for( usize i = 0; i < out_last->len; ++i ) {
        if( !char_is_whitespace( out_last->buffer[i] ) ) {
            out_last->buffer   += i;
            out_last->len      -= i;
            out_last->capacity -= i;
            break;
        }
    }

    return true;
}
LD_API u64 string_slice_hash( StringSlice* slice ) {
    local const u64 multiplier = 97;

    u64 result = 0;
    for( usize i = 0; i < slice->len; ++i ) {
        result = result * multiplier + slice->buffer[i];
    }

    if( slice->len ) {
        result %= slice->len;
    }

    return result;
}

usize ___internal_write_string_slice( void* target, usize count, char* characters ) {
    StringSlice* target_slice = target;
    usize new_len  = target_slice->len + count;
    usize max_copy = count;
    usize result   = 0;
    if( new_len > target_slice->capacity ) {
        max_copy = target_slice->capacity - target_slice->len;
        result   = new_len - max_copy;

        new_len = target_slice->capacity;
    }

    memory_copy( target_slice->buffer + target_slice->len, characters, max_copy );
    target_slice->len = new_len;

    return result;
}

LD_API usize ___internal_string_slice_fmt_va(
    StringSlice* slice, usize format_len, const char* format, va_list va
) {
    return ___internal_fmt_write_va(
        ___internal_write_string_slice, slice, format_len, format, va );
}
LD_API usize ___internal_string_slice_fmt(
    StringSlice* slice, usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );

    usize result = ___internal_fmt_write_va(
        ___internal_write_string_slice, slice, format_len, format, va );

    va_end( va );

    return result;
}
LD_API usize string_slice_fmt_bool( StringSlice* slice, b32 b, b32 binary ) {
    return fmt_write_bool( ___internal_write_string_slice, slice, b, binary );
}
LD_API usize string_slice_fmt_float( StringSlice* slice, f64 f, u32 precision ) {
    return fmt_write_float( ___internal_write_string_slice, slice, f, precision );
}
LD_API usize string_slice_fmt_i8( StringSlice* slice, i8 i, enum FormatInteger format ) {
    return fmt_write_i8( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_u8( StringSlice* slice, u8 i, enum FormatInteger format ) {
    return fmt_write_u8( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_i16( StringSlice* slice, i16 i, enum FormatInteger format ) {
    return fmt_write_i16( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_u16( StringSlice* slice, u16 i, enum FormatInteger format ) {
    return fmt_write_u16( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_i32( StringSlice* slice, i32 i, enum FormatInteger format ) {
    return fmt_write_i32( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_u32( StringSlice* slice, u32 i, enum FormatInteger format ) {
    return fmt_write_u32( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_i64( StringSlice* slice, i64 i, enum FormatInteger format ) {
    return fmt_write_i64( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_u64( StringSlice* slice, u64 i, enum FormatInteger format ) {
    return fmt_write_u64( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_isize(
    StringSlice* slice, isize i, enum FormatInteger format
) {
    return fmt_write_isize( ___internal_write_string_slice, slice, i, format );
}
LD_API usize string_slice_fmt_usize(
    StringSlice* slice, usize i, enum FormatInteger format
) {
    return fmt_write_usize( ___internal_write_string_slice, slice, i, format );
}

