/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "core/strings.h"
#include "core/log.h"
#include "core/memoryf.h"
#include "core/cstr.h"

#include "core/mathf.h"

#include "internal.h"

LD_API void char_output_stdout( char character ) {
    platform->io.console_write( platform->io.stdout_handle(), 1, &character );
}
LD_API void char_output_stderr( char character ) {
    platform->io.console_write( platform->io.stderr_handle(), 1, &character );
}
hot LD_API void ss_output_stdout( StringSlice* slice ) {
    platform->io.console_write(
        platform->io.stdout_handle(),
        slice->len, slice->buffer
    );
}
hot LD_API void ss_output_stderr( StringSlice* slice ) {
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
LD_API b32 ss_parse_int( StringSlice* slice, i64* out_integer ) {
    if( !slice->len ) {
        return false;
    }

    b32 is_negative = false;
    i64 result = 0;

    usize at = 0;
    StringSlice parse = ss_clone( slice );
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
LD_API b32 ss_parse_uint( StringSlice* slice, u64* out_integer ) {
    if( !slice->len ) {
        return false;
    }

    u64 result = 0;

    usize at = 0;
    StringSlice parse = ss_clone( slice );

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

LD_API b32 ss_parse_float( StringSlice* slice, f64* out_float ) {
    // f64 result = 0.0;
    i64 whole_part      = 0;
    u64 fractional_part = 0;

    usize dot_position = 0;
    if(
        ss_find_char( slice, '.', &dot_position ) &&
        dot_position + 1 < slice->len
    ) {
        StringSlice first = {}, last = {};
        ss_split_at( slice, dot_position, &first, &last );
        if( !ss_parse_int( &first, &whole_part ) ) {
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
            if( !ss_parse_uint( &last, &fractional_part ) ) {
                return false;
            }
        }

        *out_float = (f64)whole_part;
        f64 fractional_part_f64 = (f64)fractional_part;
        u64 places = ___places( fractional_part );

        u64 power = places + zero_count;
        if( power ) {
            fractional_part_f64 /= powi64( 10.0, power );
        }

        *out_float += fractional_part_f64;

        return true;
    } else {
        b32 success = ss_parse_int( slice, &whole_part );
        if( success ) {
            *out_float = (f64)whole_part;
            return success;
        } else {
            return success;
        }
    }
}
LD_API StringSlice ss_from_cstr( usize opt_len, const char* cstr ) {
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
LD_API b32 ss_cmp( StringSlice* a, StringSlice* b ) {
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
LD_API b32 ss_find(
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

        if( ss_cmp( &remaining, phrase ) ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }
            return true;
        }
    }

    return false;

}
LD_API b32 ss_find_char(
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
LD_API usize ss_phrase_count( StringSlice* slice, StringSlice* phrase ) {
    usize count = 0;
    if( slice->len < phrase->len ) {
        return 0;
    }

    for( usize i = 0; i < slice->len; ++i ) {
        usize remaining_len = slice->len - i;
        if( remaining_len < phrase->len ) {
            return count;
        }

        if( slice->buffer[i] == phrase->buffer[0] ) {
            StringSlice remaining;
            remaining.buffer = slice->buffer + i;
            remaining.len    = phrase->len;
            remaining.capacity = remaining.len;

            if( ss_cmp( &remaining, phrase ) ) {
                count++;
            }
        }
    }

    return count;
}
LD_API usize ss_char_count( StringSlice* slice, char character ) {
    usize count = 0;
    for( usize i = 0; i < slice->len; ++i ) {
        if( slice->buffer[i] == character ) {
            count++;
        }
    }
    return count;
}
LD_API void ss_mut_copy( StringSlice* dst, StringSlice* src ) {
    assert( dst->capacity );

    usize max_copy = dst->capacity > src->len ?
        src->len : dst->capacity;
    mem_copy( dst->buffer, src->buffer, max_copy );

    if( max_copy > dst->len ) {
        dst->len = max_copy;
    }
}
LD_API void ss_mut_copy_to_len( StringSlice* dst, StringSlice* src ) {
    usize max_copy = dst->len > src->len ?
        src->len : dst->len;
    mem_copy( dst->buffer, src->buffer, max_copy );
}
LD_API void ss_mut_copy_cstr(
    StringSlice* dst, usize opt_len, const char* src
) {
    StringSlice cstr;
    cstr.buffer = (char*)src;
    if( opt_len ) {
        cstr.len = opt_len;
    } else {
        cstr.len = cstr_len( src );
    }
    ss_mut_copy( dst, &cstr );
}
LD_API void ss_mut_copy_cstr_to_len(
    StringSlice* dst, usize opt_len, const char* src
) {
    StringSlice cstr;
    cstr.buffer = (char*)src;
    if( opt_len ) {
        cstr.len = opt_len;
    } else {
        cstr.len = cstr_len( src );
    }
    ss_mut_copy_to_len( dst, &cstr );
}
LD_API void ss_mut_reverse( StringSlice* slice ) {
    if( !slice->buffer || !slice->len || slice->len == 1 ) {
        return;
    }

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
LD_API void ss_mut_trim_trailing_whitespace( StringSlice* slice ) {
    for( usize i = slice->len; i-- > 0; ) {
        char current = slice->buffer[i];
        if( current != ' ' || current != '\t' || current != '\n' ) {
            slice->len = i;
            return;
        }
    }
}
LD_API void ss_mut_fill( StringSlice* slice, char character ) {
    mem_set( slice->buffer, character, slice->len );
}
LD_API void ss_mut_fill_to_capacity( StringSlice* slice, char character ) {
    mem_set( slice->buffer, character, slice->capacity );
}
LD_API b32 ss_mut_push( StringSlice* slice, char character ) {
    if( slice->len == slice->capacity ) {
        return false;
    }

    slice->buffer[slice->len++] = character;

    return true;
}
LD_API b32 ss_mut_pop( StringSlice* slice, char* opt_out_char ) {
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
LD_API b32 ss_mut_insert(
    StringSlice* slice, char character, usize position
) {
    if( position == slice->len ) {
        return ss_mut_push( slice, character );
    }
    if( slice->len == slice->capacity ) {
        return false;
    }

    usize remaining_len = slice->len - position;
    mem_copy_overlapped(
        slice->buffer + position + 1,
        slice->buffer + position,
        remaining_len
    );
    slice->buffer[position] = character;
    slice->len++;

    return true;

}
LD_API b32 ss_mut_append( StringSlice* slice, StringSlice* append ) {
    b32 full_append = slice->capacity >= append->len;

    usize max_copy = append->len > slice->capacity ?
        slice->capacity - append->len : append->len;
    
    if( !max_copy ) {
        return false;
    }

    mem_copy(
        slice->buffer + slice->len,
        append->buffer, max_copy );

    slice->len += max_copy;
    return full_append;
}
LD_API void ss_split_at(
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
LD_API b32 ss_split_at_whitespace(
    StringSlice* slice_to_split,
    StringSlice* out_first, StringSlice* out_last
) {
    usize index = 0;
    if( !ss_find_char( slice_to_split, ' ', &index ) ) {
        if( !ss_find_char( slice_to_split, '\t', &index ) ) {
            if( !ss_find_char( slice_to_split, '\n', &index ) ) {
                return false;
            }
        }
    }

    ss_split_at( slice_to_split, index, out_first, out_last );

    return true;
}
LD_API u64 ss_hash( StringSlice* slice ) {
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

global char BINARY_DIGITS[2] = { '0', '1' };
global char DECIMAL_DIGITS[10] = {
    '0', '1', '2',
    '3', '4', '5',
    '6', '7', '8',
    '9'
};
global char HEX_DIGITS[16] = {
    '0', '1', '2',
    '3', '4', '5',
    '6', '7', '8',
    '9', 'A', 'B',
    'C', 'D', 'E',
    'F'
};
#define DECIMAL_BASE 10
#define HEX_BASE     16

#define ___push_slice( c ) do {\
    if( !slice || !ss_mut_push( slice, c ) ) {\
        result++;\
    }\
} while(0)

LD_API usize ss_mut_fmt_i8(
    StringSlice* slice, i8 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u8 abs;
    if( value < 0 ) {
        abs = (u8)( value * -1 );
    } else {
        abs = (u8)(value);
    }

    u8 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( abs ) {
        u8 digit = abs % base;
        ___push_slice( digits[digit] );
        abs /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_DECIMAL: {
            if( value < 0 ) {
                ___push_slice( '-' );
            }
        } break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}
LD_API usize ss_mut_fmt_i16(
    StringSlice* slice, i16 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u16 abs;
    if( value < 0 ) {
        abs = (u16)( value * -1 );
    } else {
        abs = (u16)(value);
    }

    u16 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( abs ) {
        u16 digit = abs % base;
        ___push_slice( digits[digit] );
        abs /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_DECIMAL: {
            if( value < 0 ) {
                ___push_slice( '-' );
            }
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}
LD_API usize ss_mut_fmt_i32(
    StringSlice* slice, i32 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u32 abs;
    if( value < 0 ) {
        abs = (u32)( value * -1 );
    } else {
        abs = (u32)(value);
    }

    u32 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( abs ) {
        u32 digit = abs % base;
        ___push_slice( digits[digit] );
        abs /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_DECIMAL: {
            if( value < 0 ) {
                ___push_slice( '-' );
            }
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;

}
LD_API usize ss_mut_fmt_i64(
    StringSlice* slice, i64 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u64 abs;
    if( value < 0 ) {
        abs = (u64)( value * -1 );
    } else {
        abs = (u64)(value);
    }

    u64 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( abs ) {
        u64 digit = abs % base;
        ___push_slice( digits[digit] );
        abs /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_DECIMAL: {
            if( value < 0 ) {
                ___push_slice( '-' );
            }
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}

LD_API usize ss_mut_fmt_u8(
    StringSlice* slice, u8 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u8 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( value ) {
        u8 digit = value % base;
        ___push_slice( digits[digit] );
        value /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}
LD_API usize ss_mut_fmt_u16(
    StringSlice* slice, u16 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u16 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( value ) {
        u16 digit = value % base;
        ___push_slice( digits[digit] );
        value /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}
LD_API usize ss_mut_fmt_u32(
    StringSlice* slice, u32 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u32 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( value ) {
        u32 digit = value % base;
        ___push_slice( digits[digit] );
        value /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}
LD_API usize ss_mut_fmt_u64(
    StringSlice* slice, u64 value, FormatInteger fmt
) {
    usize result = 0;
    if( !value ) {
        ___push_slice( '0' );
        return result;
    }
    u64 base;
    char* digits;

    switch( fmt ) {
        case FMT_INT_BINARY: {
            base = 2;
            digits = BINARY_DIGITS;
        } break;
        case FMT_INT_HEX: {
            base = 16;
            digits = HEX_DIGITS;
        } break;
        case FMT_INT_DECIMAL: {
            base = 10;
            digits = DECIMAL_DIGITS;
        } break;
    }

    while( value ) {
        u64 digit = value % base;
        ___push_slice( digits[digit] );
        value /= base;
    }

    switch( fmt ) {
        case FMT_INT_BINARY: {
            ___push_slice( 'b' );
            ___push_slice( '0' );
        } break;
        case FMT_INT_HEX: {
            ___push_slice( 'x' );
            ___push_slice( '0' );
        } break;
        default: break;
    }

    if( slice ) {
        ss_mut_reverse( slice );
    }

    return result;
}

LD_API usize ss_mut_fmt_f32( StringSlice* slice, f32 value, u32 precision ) {
    return ss_mut_fmt_f64( slice, (f64)value, precision );
}
LD_API usize ss_mut_fmt_f64(
    StringSlice* slice, f64 value, u32 precision
) {
    usize result = 0;
    if( is_nan64( value ) ) {
        ___push_slice( 'N' );
        ___push_slice( 'a' );
        ___push_slice( 'N' );
        return result;
    } else if( value == F64_POS_INFINITY ) {
        ___push_slice( 'I' );
        ___push_slice( 'N' );
        ___push_slice( 'F' );
        return result;
    } else if( value == F64_NEG_INFINITY ) {
        ___push_slice( '-' );
        ___push_slice( 'I' );
        ___push_slice( 'N' );
        ___push_slice( 'F' );
        return result;
    }

    f64 abs;
    if( value < 0 ) {
        abs = value * -1.0;
    } else {
        abs = value;
    }

    u64   base   = 10;
    char* digits = DECIMAL_DIGITS;

    u64 whole_part = (u64)abs;
    f64 fractional = abs - (f64)whole_part;

    if( !whole_part ) {
        ___push_slice( digits[0] );
    } else while( whole_part ) {
        u64 digit = whole_part % base;
        ___push_slice( digits[digit] );
        whole_part /= base;
    }

    u32 precision_left = min( precision, 10 );
    fractional *= 10.0;
    whole_part = (u64)(fractional);

    if( value < 0 ) {
        ___push_slice( '-' );
    }
    usize rev_end = 0;
    if( slice ) {
        rev_end = slice->len;
    }

    if( precision_left ) {
        ___push_slice( '.' );
    }

    while( precision_left ) {
        u64 digit = whole_part % base;
        ___push_slice( digits[digit] );
        fractional *= 10.0;
        whole_part = (u64)(fractional);
        precision_left--;
    }

    if( slice ) {
        StringSlice rev;
        rev.buffer = slice->buffer;
        rev.len    = rev_end;
        ss_mut_reverse( &rev );
    }

    return result;
}
LD_API usize ss_mut_fmt_b32( StringSlice* slice, b32 value ) {
    ss_const( ss_true, "true" );
    ss_const( ss_false, "false" );

    StringSlice* value_slice = value ? &ss_true : &ss_false;

    usize result = 0;

    for( usize i = 0; i < value_slice->len; ++i ) {
        ___push_slice( value_slice->buffer[i] );
    }

    return result;
}

typedef enum {
    FMT_STORAGE_BYTES,
    FMT_STORAGE_KB   ,
    FMT_STORAGE_MB   ,
    FMT_STORAGE_GB   ,
    FMT_STORAGE_TB   ,
} FormatFloatStorageType;
maybe_unused
internal f64 ___determine_storage( f64 f, FormatFloatStorageType* out_type ) {
    f64 result = f;
    *out_type = FMT_STORAGE_BYTES;
    if( result >= 1024.0 ) {
        *out_type = FMT_STORAGE_KB;
        result /= 1024.0;
        if( result >= 1024.0 ) {
            *out_type = FMT_STORAGE_MB;
            result /= 1024.0;
            if( result >= 1024.0 ) {
                *out_type = FMT_STORAGE_GB;
                result /= 1024.0;
                if( result >= 1024.0 ) {
                    *out_type = FMT_STORAGE_TB;
                    result /= 1024.0;
                }
            }
        }
    }
    return result;
}

typedef b32 PutCharFN( StringSlice*, char );

maybe_unused
no_inline internal b32 ___put_slice( StringSlice* slice, char c ) {
    if( !slice ) {
        return false;
    }
    return ss_mut_push( slice, c );
}
maybe_unused
no_inline internal b32 ___put_stdout( StringSlice* _, char c ) {
    unused(_);
    char_output_stdout( c );
    return true;
}
maybe_unused
no_inline internal b32 ___put_stderr( StringSlice* _, char c ) {
    unused(_);
    char_output_stderr( c );
    return true;
}

internal hot b32 ___precision_dot( const char* outer_at, usize* out_offset ) {
    usize offset = 0;
    const char* at = outer_at;
    while( *at ) {
        if( *at == ',' || *at == '}' ) {
            return false;
        }
        if( *at == '.' ) {
            if( out_offset ) {
                *out_offset = offset;
            }
            return true;
        }
        at++;
        offset++;
    }
    return false;
}
internal hot b32 ___closing_brace( const char* outer_at, usize* out_offset ) {
    usize offset = 0;
    const char* at = outer_at;
    while( *at ) {
        if( *at == '}' || *at == ',' ) {
            if( out_offset ) {
                *out_offset = offset;
            }
            return true;
        }
        at++;
        offset++;
    }
    return false;
}
typedef enum : u32 {
    FMT_INT_PRECISION_8,
    FMT_INT_PRECISION_16,
    FMT_INT_PRECISION_32,
    FMT_INT_PRECISION_64,
} FormatIntegerPrecision;

typedef union {
    i32 _vector[4];
} FormatIntegerValues;

no_inline hot
internal usize ___fmt(
    StringSlice* slice,
    const char* format,
    PutCharFN* put,
    va_list va
) {
    
    #define ___intermediate_buffer_size 64
    char intermediate_buffer[___intermediate_buffer_size] = {};
    StringSlice intermediate = {};
    intermediate.buffer   = intermediate_buffer;
    intermediate.capacity = ___intermediate_buffer_size;

    ss_const( ss_true, "true" );
    ss_const( ss_false, "false" );

    usize result = 0;
    
    #define ___incr() do {\
        at++;\
        if( !at ) {\
            goto fmt_end;\
        }\
    } while(0)

    #define ___put( c ) do {\
        if( !put( slice, c ) ) {\
            result++;\
        }\
    } while(0)

    const char* at = format;
    while( *at ) {
        if( *at != '{' ) {
            if( *at == '}' ) {
                ___incr();
                ___put( '}' );
                if( *at == '}' ) {
                    ___incr();
                }
                continue;
            }
            ___put( *at++ );
            continue;
        }
        ___incr();

        b32 is_signed                        = false;
        char padding_char                    = ' ';
        i64  padding                         = 0;
        FormatInteger fmt                    = FMT_INT_DECIMAL;
        FormatIntegerPrecision int_precision = FMT_INT_PRECISION_32;
        u32 component_count                  = 1;

        switch( *at ) {

            case '{': {
                ___put( *at );
                ___incr();
            } break;

            case 'b':
            case 'B': {
                ___incr();
                int val = va_arg( va, int );

                b32 fmt_binary = false;
                padding = 0;

                while( *at == ',' ) {
                    ___incr();
                    if( *at == 'b' || *at == 'B' ) {
                        fmt_binary = true;
                        ___incr();
                        continue;
                    }

                    usize end = 0;
                    if( ___closing_brace( at, &end ) ) {
                        StringSlice number;
                        number.buffer = (char*)at;
                        number.len    = end;

                        if( !ss_parse_int( &number, &padding ) ) {
                            goto fmt_end;
                        }

                        at = at + end;
                    } else {
                        goto fmt_end;
                    }
                }

                intermediate.len = 0;
                if( fmt_binary ) {
                    StringSlice bin;
                    bin.buffer = val ? "1" : "0";
                    bin.len    = 1;
                    ss_mut_copy( &intermediate, &bin );
                } else {
                    ss_mut_copy(
                        &intermediate, val ? &ss_true : &ss_false );
                }

                if( padding > 0 ) {
                    i32 pad_count = padding - intermediate.len;
                    for( i32 i = 0; i < pad_count; ++i ) {
                        ___put( ' ' );
                    }
                    padding = 0;
                }

                for( usize i = 0; i < intermediate.len; ++i ) {
                    ___put( intermediate.buffer[i] );
                }

                if( padding < 0 ) {
                    padding *= -1;
                    i32 pad_count = padding - intermediate.len;
                    for( i32 i = 0; i < pad_count; ++i ) {
                        ___put( ' ' );
                    }
                }

                ___incr();
            } break;

            case 'c':
            case 'C': {
                ___incr();
                b32 is_string = false;
                if( *at == 'c' || *at == 'C' ) {
                    is_string = true;
                    ___incr();
                }

                padding = 0;

                while( *at == ',' ) {
                    ___incr();
                    usize end = 0;
                    if( ___closing_brace( at, &end ) ) {
                        StringSlice number;
                        number.buffer = (char*)at;
                        number.len    = end;

                        if( !ss_parse_int( &number, &padding ) ) {
                            goto fmt_end;
                        }

                        at = at + end;
                    } else {
                        goto fmt_end;
                    }
                }

                if( is_string ) {
                    const char* val = va_arg( va, const char* );
                    if( !val ) {
                        goto fmt_end;
                    }
                    usize val_len = 0;
                    if( padding ) {
                        val_len = cstr_len( val );
                    }

                    if( padding > 0 ) {
                        i32 pad_count = padding - val_len;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( ' ' );
                        }
                        padding = 0;
                    }


                    while( *val ) {
                        ___put( *val++ );
                    }

                    if( padding < 0 ) {
                        padding *= -1;
                        i32 pad_count = padding - val_len;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( ' ' );
                        }
                    }
                } else {
                    int val = va_arg( va, int );

                    if( padding > 0 ) {
                        i32 pad_count = padding - 1;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( ' ' );
                        }
                        padding = 0;
                    }

                    ___put( (char)val );

                    if( padding < 0 ) {
                        padding *= -1;
                        i32 pad_count = padding - 1;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( ' ' );
                        }
                    }
                }

                ___incr();
            } break;

            case 'S':
            case 's': {
                ___incr();

                padding = 0;

                while( *at == ',' ) {
                    ___incr();
                    usize end = 0;
                    if( ___closing_brace( at, &end ) ) {
                        StringSlice number;
                        number.buffer = (char*)at;
                        number.len    = end;

                        if( !ss_parse_int( &number, &padding ) ) {
                            goto fmt_end;
                        }

                        at = at + end;
                    } else {
                        goto fmt_end;
                    }
                }

                StringSlice val = va_arg( va, StringSlice );

                if( padding > 0 ) {
                    i32 pad_count = padding - val.len;
                    for( i32 i = 0; i < pad_count; ++i ) {
                        ___put( ' ' );
                    }
                    padding = 0;
                }

                for( usize i = 0; i < val.len; ++i ) {
                    ___put( val.buffer[i] );
                }

                if( padding < 0 ) {
                    padding *= -1;
                    i32 pad_count = padding - val.len;
                    for( i32 i = 0; i < pad_count; ++i ) {
                        ___put( ' ' );
                    }
                }

                ___incr();
            } break;

            case 'i':
            case 'I':
                ___incr();

                is_signed = true;
                if( !(*at == 'v' || *at == 'V') ) {
                    goto fmt_int;
                }

                ___incr();

                component_count = 0;
                if( *at == '2' ) {
                    component_count = 2;
                } else if( *at == '3' ) {
                    component_count = 3;
                } else if( *at == '4' ) {
                    component_count = 4;
                } else {
                    goto fmt_end;
                }

            case 'U':
            case 'u': {
                ___incr();

            fmt_int:
                padding_char  = ' ';
                padding       = 0;
                fmt           = FMT_INT_DECIMAL;
                int_precision = FMT_INT_PRECISION_32;

                while(
                    component_count == 1 &&
                    *at != ',' &&
                    *at != '}'
                ) {
                    if( *at == '8' ) {
                        int_precision = FMT_INT_PRECISION_8;
                        ___incr();
                        if( !(*at == ',' || *at == '}') ) {
                            goto fmt_end;
                        }
                        continue;
                    }
                    if( *at == '1' ) {
                        ___incr();
                        if( *at == '6' ) {
                            int_precision = FMT_INT_PRECISION_16;
                            ___incr();
                            if( !(*at == ',' || *at == '}') ) {
                                goto fmt_end;
                            }
                            continue;
                        } else {
                            goto fmt_end;
                        }
                    }
                    if( *at == '3' ) {
                        ___incr();
                        if( *at == '2' ) {
                            int_precision = FMT_INT_PRECISION_32;
                            ___incr();
                            if( !(*at == ',' || *at == '}') ) {
                                goto fmt_end;
                            }
                            continue;
                        } else {
                            goto fmt_end;
                        }
                    }
                    if( *at == '6' ) {
                        ___incr();
                        if( *at == '4' ) {
                            int_precision = FMT_INT_PRECISION_64;
                            ___incr();
                            if( !(*at == ',' || *at == '}') ) {
                                goto fmt_end;
                            }
                            continue;
                        } else {
                            goto fmt_end;
                        }
                    }
                    char size_word[4] = { 's', 'i', 'z', 'e' };
                    for(
                        u32 i = 0;
                        i < static_array_count(size_word);
                        ++i
                    ) {
                        if( *at == size_word[i] ) {
                            ___incr();
                        } else {
                            goto fmt_end;
                        }
                    }
                    #if defined(LD_ARCH_64_BIT)
                        int_precision = FMT_INT_PRECISION_64;
                    #else
                        int_precision = FMT_INT_PRECISION_32;
                    #endif
                }

                while( *at == ',' ) {
                    ___incr();

                    if( *at == 'b' ) {
                        fmt = FMT_INT_BINARY;
                        ___incr();
                        continue;
                    }

                    if( *at == 'x' ) {
                        fmt = FMT_INT_HEX;
                        ___incr();
                        continue;
                    }

                    usize end = 0;
                    if( ___closing_brace( at, &end ) ) {
                        StringSlice number;
                        number.buffer = (char*)at;
                        number.len    = end;

                        if( *at == '0' ) {
                            padding_char = '0';
                        }

                        if( !ss_parse_int( &number, &padding ) ) {
                            goto fmt_end;
                        }

                        at = at + end;
                    } else {
                        goto fmt_end;
                    }
                }

                if( component_count > 1 ) {
                    ___put( '{' );
                    ___put( ' ' );
                }

                intermediate.len = 0;
                FormatIntegerValues values = {};
                if( component_count == 1 ) {
                    switch( int_precision ) {
                        case FMT_INT_PRECISION_8: {
                            int val_ = va_arg( va, int );
                            if( is_signed ) {
                                i8 val = (i8)val_;
                                ss_mut_fmt_i8(
                                    &intermediate, val, fmt );
                            } else {
                                u8 val = (u8)reinterpret( u32, val_ );
                                ss_mut_fmt_u8(
                                    &intermediate, val, fmt );
                            }
                        } break;
                        case FMT_INT_PRECISION_16: {
                            int val_ = va_arg( va, int );
                            if( is_signed ) {
                                i16 val = (i16)val_;
                                ss_mut_fmt_i16(
                                    &intermediate, val, fmt );
                            } else {
                                u16 val = (u16)reinterpret( u32, val_ );
                                ss_mut_fmt_u16(
                                    &intermediate, val, fmt );
                            }
                        } break;
                        case FMT_INT_PRECISION_32: {
                            int val_ = va_arg( va, int );
                            if( is_signed ) {
                                i32 val = (i32)val_;
                                ss_mut_fmt_i32(
                                    &intermediate, val, fmt );
                            } else {
                                u32 val = (u32)reinterpret( u32, val_ );
                                ss_mut_fmt_u32(
                                    &intermediate, val, fmt );
                            }
                        } break;
                        case FMT_INT_PRECISION_64: {
                            long long val_ = va_arg( va, long long );
                            if( is_signed ) {
                                i64 val = (i64)val_;
                                ss_mut_fmt_i64(
                                    &intermediate, val, fmt );
                            } else {
                                u64 val = (u64)reinterpret( u64, val_ );
                                ss_mut_fmt_u64(
                                    &intermediate, val, fmt );
                            }
                        } break;
                    }
                } else {
                    switch( component_count ) {
                        case 2: {
                            ivec2 val_ = va_arg( va, ivec2 );
                            values._vector[0] = val_.x;
                            values._vector[1] = val_.y;
                        } break;
                        case 3: {
                            ivec3 val_ = va_arg( va, ivec3 );
                            values._vector[0] = val_.x;
                            values._vector[1] = val_.y;
                            values._vector[2] = val_.z;
                        } break;
                        case 4: {
                            ivec4 val_ = va_arg( va, ivec4 );
                            values._vector[0] = val_.x;
                            values._vector[1] = val_.y;
                            values._vector[2] = val_.z;
                            values._vector[3] = val_.w;
                        } break;
                        default: goto fmt_end;
                    }
                }

                for( u32 i = 0; i < component_count; ++i ) {
                    i32 padding_ = padding;
                    if( component_count > 1 ) {
                        intermediate.len = 0;
                        ss_mut_fmt_i32(
                            &intermediate, values._vector[i], fmt );
                    }

                    if( padding_ > 0 ) {
                        i32 pad_count = padding_ - intermediate.len;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( padding_char );
                        }
                        padding_ = 0;
                    }

                    for( usize i = 0; i < intermediate.len; ++i ) {
                        ___put( intermediate.buffer[i] );
                    }

                    if( padding_ < 0 ) {
                        padding_ *= -1;
                        i32 pad_count = padding_ - intermediate.len;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( padding_char );
                        }
                    }

                    if(
                        component_count > 1 &&
                        i + 1 < component_count
                    ) {
                        ___put( ',' );
                        ___put( ' ' );
                    }
                }

                if( component_count > 1 ) {
                    ___put( ' ' );
                    ___put( '}' );
                }

                ___incr();
            } break;

            case 'q':
            case 'Q':
                component_count = 4;
            case 'v':
            case 'V':
                if( component_count == 1 ) {
                    ___incr();
                    switch( *at ) {
                        case '2': {
                            component_count = 2;
                        } break;
                        case '3': {
                            component_count = 3;
                        } break;
                        case '4': {
                            component_count = 4;
                        } break;
                        default: goto fmt_end;
                    }
                }
            case 'f':
            case 'F': {
                ___incr();

                u64 precision   = 0;
                b32 fmt_storage = false;
                padding_char    = ' ';
                padding         = 0;
                while( *at == ',' ) {
                    ___incr();

                    if( *at == 'b' ) {
                        if( component_count != 1 ) {
                            goto fmt_end;
                        }
                        fmt_storage = true;
                        ___incr();
                        continue;
                    }

                    usize padding_end = 0;
                    if( ___precision_dot( at, &padding_end ) ) {
                        StringSlice number;
                        if( padding_end ) {
                            number.buffer = (char*)at;
                            number.len    = padding_end;

                            if( *at == '0' ) {
                                padding_char = '0';
                            }

                            if( !ss_parse_int( &number, &padding ) ) {
                                goto fmt_end;
                            }

                        }
                        at = at + (padding_end ? padding_end + 1 : 1);

                        usize precision_end = 0;
                        if( ___closing_brace( at, &precision_end ) ) {
                            number.buffer = (char*)at;
                            number.len    = precision_end;

                            if(
                                !ss_parse_uint( &number, &precision )
                            ) {
                                goto fmt_end;
                            }
                            at = at + precision_end;
                        } else {
                            goto fmt_end;
                        }
                    } else if(
                        ___closing_brace( at, &padding_end )
                    ) {
                        StringSlice number;
                        number.buffer = (char*)at;
                        number.len    = padding_end;

                        if( *at == '0' ) {
                            padding_char = '0';
                        }

                        if( !ss_parse_int( &number, &padding ) ) {
                            goto fmt_end;
                        }

                        at = at + padding_end;
                    } else {
                        goto fmt_end;
                    }

                }

                precision = precision ? precision : 6;
                intermediate.len = 0;

                FormatFloatStorageType storage_type;

                usize padding_offset = 0;

                f64 values[4] = {};
                switch( component_count ) {
                    case 1: {
                        f64 val = va_arg( va, double );
                        if( fmt_storage ) {
                            val = ___determine_storage(
                                val, &storage_type );
                        }
                        values[0] = val;
                    } break;
                    case 2: {
                        vec2 val = va_arg( va, vec2 );
                        values[0] = val.x;
                        values[1] = val.y;
                    } break;
                    case 3: {
                        vec3 val = va_arg( va, vec3 );
                        values[0] = val.x;
                        values[1] = val.y;
                        values[2] = val.z;
                    } break;
                    case 4: {
                        vec4 val = va_arg( va, vec4 );
                        values[0] = val.x;
                        values[1] = val.y;
                        values[2] = val.z;
                        values[3] = val.w;
                    } break;
                    default: break;
                }

                if( component_count > 1 ) {
                    ___put( '{' );
                    ___put( ' ' );
                }
                for( u32 i = 0; i < component_count; ++i ) {
                    intermediate.len = 0;

                    ss_mut_fmt_f64(
                        &intermediate,
                        values[i],
                        precision
                    );

                    if(
                        intermediate.len >= precision &&
                        padding > 0
                    ) {
                        padding_offset =
                            intermediate.len - precision;
                    } else {
                        padding_offset = intermediate.len;
                    }

                    padding_offset = padding_offset ?
                        padding_offset - 1 : padding_offset;

                    if( padding > 0 ) {
                        i32 pad_count = padding - padding_offset;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( padding_char );
                        }
                        padding = 0;
                    }

                    for( usize i = 0; i < intermediate.len; ++i ) {
                        ___put( intermediate.buffer[i] );
                    }

                    if( fmt_storage ) {
                        ___put( ' ' );
                        switch( storage_type ) {
                            case FMT_STORAGE_KB: {
                                ___put( 'K' );
                            } break;
                            case FMT_STORAGE_MB: {
                                ___put( 'M' );
                            } break;
                            case FMT_STORAGE_GB: {
                                ___put( 'G' );
                            } break;
                            case FMT_STORAGE_TB: {
                                ___put( 'T' );
                            } break;
                            default: break;
                        }
                        ___put( 'B' );
                    }

                    if( padding < 0 ) {
                        padding *= -1;
                        i32 pad_count = padding - padding_offset;
                        for( i32 i = 0; i < pad_count; ++i ) {
                            ___put( ' ' );
                        }
                    }

                    if(
                        component_count > 1 &&
                        i + 1 != component_count
                    ) {
                        ___put( ',' );
                        ___put( ' ' );
                    }
                }

                if( component_count > 1 ) {
                    ___put( ' ' );
                    ___put( '}' );
                }

                ___incr();
            } break;
        }

    }

fmt_end:
    return result;
}

LD_API usize ss_mut_fmt_va(
    StringSlice* buffer,
    const char* format,
    va_list va
) {
    return ___fmt( buffer, format, ___put_slice, va );
}
LD_API usize ss_mut_fmt( StringSlice* buffer, const char* format, ... ) {
    va_list va;
    va_start( va, format );

    usize result = ss_mut_fmt_va( buffer, format, va );

    va_end( va );
    return result;
}

LD_API void print( const char* format, ... ) {
    va_list va;
    va_start( va, format );

    ___fmt( NULL, format, ___put_stdout, va );

    va_end( va );

    ___put_stdout( NULL, 0 );
}
LD_API void print_err( const char* format, ... ) {
    va_list va;
    va_start( va, format );

    ___fmt( NULL, format, ___put_stderr, va );

    va_end( va );

    ___put_stderr( NULL, 0 );
}
LD_API void print_va( const char* format, va_list va ) {
    ___fmt( NULL, format, ___put_stdout, va );
    ___put_stdout( NULL, 0 );
}
LD_API void print_err_va( const char* format, va_list va ) {
    ___fmt( NULL, format, ___put_stderr, va );
    ___put_stderr( NULL, 0 );
}

