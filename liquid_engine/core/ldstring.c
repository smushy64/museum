/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "core/ldstring.h"
#include "core/ldlog.h"
#include "core/ldmemory.h"
#include "core/ldcstr.h"

#include "core/ldmath.h"

#include "ldplatform.h"

LD_API void char_output_stdout( char character ) {
    platform_write_console( platform_stdout_handle(), 1, &character );
}
LD_API void char_output_stderr( char character ) {
    platform_write_console( platform_stderr_handle(), 1, &character );
}
internal i32 parse_i32_internal( char** at_init ) {
    b32 is_negative = false;
    i32 result = 0;
    char* at = *at_init;
    if( *at == '-' ) {
        ++at;
        is_negative = true;
    }
    while( (*at >= '0') && (*at <= '9') ) {
        result *= 10;
        result += (*at - '0');
        ++at;
    }
    *at_init = at;
    return result * (is_negative ? -1 : 1);
}
hot LD_API void ss_output_stdout( StringSlice* slice ) {
    platform_write_console(
        platform_stdout_handle(),
        slice->len, slice->buffer
    );
}
hot LD_API void ss_output_stderr( StringSlice* slice ) {
    platform_write_console(
        platform_stderr_handle(),
        slice->len, slice->buffer
    );
}
LD_API b32 ss_parse_i32( StringSlice* slice, i32* out_integer ) {
    // TODO(alicia): rewrite!
    i32 result = parse_i32_internal( (char**)&slice->buffer );
    *out_integer = result;
    return true;
}
LD_API b32 ss_parse_u32( StringSlice* slice, u32* out_integer ) {
    // TODO(alicia): rewrite!
    i32 result = parse_i32_internal( (char**)&slice->buffer );
    *out_integer = REINTERPRET( u32, result );
    return true;
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
    ASSERT( dst->capacity );

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
    // TODO(alicia): maybe do this by 64-bit integers?
    for( usize i = 0; i < slice->len; ++i ) {
        slice->buffer[i] = character;
    }
}
LD_API void ss_mut_fill_to_capacity( StringSlice* slice, char character ) {
    for( usize i = 0; i < slice->capacity; ++i ) {
        slice->buffer[i] = character;
    }
    slice->len = slice->capacity;
}
LD_API b32 ss_mut_push( StringSlice* slice, char character ) {
    if( slice->len == slice->capacity ) {
        return false;
    }

    slice->buffer[slice->len++] = character;

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
    ASSERT( index + 1 < slice_to_split->len );

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

    result %= slice->len;

    return result;
}


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
#define HEX_BASE 16
internal u32 int_to_string(
    StringSlice slice,
    u64 value,
    u64 base,
    char* digits,
    u32 padding,
    b32 padding_is_negative,
    b32 use_zero_padding,
    b32 is_negative
) {
    if( !slice.len ) {
        return 0;
    }
    u32 slice_index = 0;
    char* slice_buffer = (char*)slice.buffer;

    if( value == 0 ) {
        slice_buffer[slice_index++] = digits[0];
    }

    while( slice_index < slice.len && value ) {
        u64 digit = value % base;
        slice_buffer[slice_index++] = digits[digit];
        value /= base;
    }
    if( base == HEX_BASE && (slice_index + 2) < slice.len ) {
        slice_buffer[slice_index++] = 'x';
        slice_buffer[slice_index++] = '0';
    }

    if( !padding_is_negative && padding && padding >= slice_index ) {
        padding = padding - slice_index;
        if( padding ) {
            padding -= is_negative;
        }
        u32 remaining_len = slice.len - slice_index;
        u32 max_len = padding > remaining_len ? remaining_len : padding;
        for( u32 i = 0; i < max_len; ++i ) {
            slice_buffer[slice_index++] =
                use_zero_padding ? '0' : ' ';
        }
    }
    return slice_index;
}

#define FLOAT_BYTE_BYTES 0
#define FLOAT_BYTE_KILO  1
#define FLOAT_BYTE_MEGA  2
#define FLOAT_BYTE_GIGA  3
typedef struct {
    u32 format;
    f64 f;
} FloatByteFormatResult;
internal FloatByteFormatResult float_byte_format( f64 f ) {
    FloatByteFormatResult result = {};
    result.format = FLOAT_BYTE_BYTES;
    if( f >= 1024.0 ) {
        result.format = FLOAT_BYTE_KILO;
        f /= 1024.0;
        if( f >= 1024.0 ) {
            result.format = FLOAT_BYTE_MEGA;
            f /= 1024.0;
            if( f >= 1024.0 ) {
                result.format = FLOAT_BYTE_GIGA;
                f /= 1024.0;
            }
        }
    }
    result.f = f;
    return result;
}

internal u32 float_to_string(
    StringSlice slice,
    f64 value,
    i32 padding,
    u32 precision,
    b32 use_zero_padding
) {
    if( !slice.len ) {
        return 0;
    }
    char* slice_buffer = (char*)slice.buffer;

    if( is_nan32( value ) ) {
        slice_buffer[0] = 'N';
        slice_buffer[1] = 'A';
        slice_buffer[2] = 'N';
        return 3;
    }

    b32 padding_is_negative = padding < 0;

    u32 write_count = 0;
    u32 integer_write_count = 0;
    u32 fract_write_count   = 0;
    u32 slice_index  = 0;

    b32 value_is_negative = value < 0.0;
    if( value_is_negative ) {
        value = -value;
    }

    u64 integer_part = trunc_i64( value );
    f64 fract_part   = value - (f64)integer_part;

    u32 max_fract_write_count = precision > slice.len ? slice.len - 1 : precision;
    for( u32 i = 0; i < max_fract_write_count; ++i ) {
        fract_part *= 10.0;
        u64 fract_part_int = clamp(trunc_i64(fract_part), 0ll, 9ll);
        fract_part -= (f64)fract_part_int;
        u32 buffer_index = (max_fract_write_count - 1) - i;
        slice_buffer[buffer_index] = DECIMAL_DIGITS[fract_part_int];
        fract_write_count++;
        slice_index++;
    }

    if( slice_index < slice.len ) {
        slice_buffer[slice_index++] = '.';
        write_count++;
    }

    if( !integer_part && slice_index < slice.len ) {
        slice_buffer[slice_index++] = '0';
        integer_write_count++;
    }
    while( slice_index < slice.len && integer_part ) {
        u64 digit = clamp((integer_part % 10ull), 0ull, 9ull);
        slice_buffer[slice_index++] = DECIMAL_DIGITS[digit];
        integer_part /= 10;
        integer_write_count++;
    }

    if( !use_zero_padding ) {
        if( value_is_negative && slice_index < slice.len ) {
            slice_buffer[slice_index++] = '-';
            write_count++;
        }
    }
    if(
        !padding_is_negative &&
        padding &&
        padding >= (i32)integer_write_count
    ) {
        padding = padding - integer_write_count;
        if( padding ) {
            padding -= value_is_negative;
        }
        u32 remaining_len = slice.len - integer_write_count;
        u32 max_len = padding > (i32)remaining_len ? remaining_len : padding;
        for( u32 i = 0; i < max_len; ++i ) {
            slice_buffer[slice_index++] =
                use_zero_padding ? '0' : ' ';
            write_count++;
        }
    }

    if( use_zero_padding ) {
        if( value_is_negative && slice_index < slice.len ) {
            slice_buffer[slice_index++] = '-';
            write_count++;
        }
    }

    write_count += integer_write_count + fract_write_count;
    return write_count;
}

typedef b32 (*WriteCharFN)( StringSlice*, char );

no_inline internal b32 write_char_dst( StringSlice* dst, char character ) {
    if( dst->len ) {
        --dst->len;
        *dst->buffer++ = character;
        return true;
    } else {
        return false;
    }
}
no_inline internal b32 write_char_stdout(
    StringSlice* slice, char character
) {
    unused(slice);
    char_output_stderr( character );
    return true;
}
no_inline internal b32 write_char_stderr(
    StringSlice* slice, char character
) {
    unused(slice);
    char_output_stderr( character );
    return true;
}

no_inline hot
internal u32 format_internal(
    StringSlice buffer,
    const char* format,
    WriteCharFN write_char_fn,
    va_list list
) {
    StringSlice dst = ss_clone( &buffer );
    if( !dst.len ) {
        u32 result = dst.buffer - buffer.buffer;
        return result;
    }

    char* at = (char*)format;
    #define TEMP_BUFFER_SIZE 64
    char temp_buffer[TEMP_BUFFER_SIZE];
    StringSlice temp_buffer_view = {};
    temp_buffer_view.buffer = temp_buffer;
    temp_buffer_view.len = TEMP_BUFFER_SIZE;

    #define CHECK_FOR_CLOSING_BRACE()\
        LOG_ASSERT(\
            *at == '}',\
            "Malformed format string \"{cc}\"! "\
            "Missing closing brace!",\
            format\
        )

    #define write_char( dst, character ) do {\
        if( !write_char_fn( dst, character ) ) {\
            quick_exit = true;\
        }\
    } while(0)

    b32 quick_exit = false;
    while( *at && !quick_exit ) {
        if( *at == '{' ) {
            ++at;
            if( *at == '{' ) {
                ++at;
                write_char( &dst, '{' );
                continue;
            }
            b32 is_unsigned  = true;
            u32 vector_count = 0;
            b32 is_quaternion = false;
            switch( *at ) {
                case 'b':
                case 'B': {
                    ++at;
                    int boolean = va_arg( list, int );
                    b32 use_binary = false;
                    b32 padding_is_negative = false;
                    i32 padding = 0;
                    while( *at == ',') {
                        ++at;
                        if( *at == 'b' || *at == 'B' ) {
                            use_binary = true;
                            ++at;
                            continue;
                        }
                        if( *at == '-' ) {
                            padding_is_negative = true;
                            ++at;
                        }
                        if( char_is_digit( *at ) ) {
                            padding = parse_i32_internal( &at );
                            continue;
                        }
                        PANIC();
                    }
                    u32 write_count = 0;
                    if( use_binary ) {
                        write_count = 1;
                    } else {
                        write_count = boolean ? 4 : 5;
                    }

                    if( !padding_is_negative && padding ) {
                        padding = padding - write_count;
                        for( i32 i = 0; i < padding; ++i ) {
                            write_char( &dst, ' ' );
                        }
                    }

                    if( use_binary ) {
                        write_char( &dst, boolean ? '1' : '0' );
                    } else {
                        if( boolean ) {
                            write_char( &dst, 't' );
                            write_char( &dst, 'r' );
                            write_char( &dst, 'u' );
                            write_char( &dst, 'e' );
                        } else {
                            write_char( &dst, 'f' );
                            write_char( &dst, 'a' );
                            write_char( &dst, 'l' );
                            write_char( &dst, 's' );
                            write_char( &dst, 'e' );
                        }
                    }
                    if( padding_is_negative && padding ) {
                        padding = padding - write_count;
                        for( i32 i = 0; i < padding; ++i ) {
                            write_char( &dst, ' ' );
                        }
                    }
                    CHECK_FOR_CLOSING_BRACE();
                } break;
                case 'C':
                case 'c': {
                    ++at;
                    if( *at == 'c' || *at == 'C' ) {
                        // const char
                        ++at;
                        char* str = va_arg(list, char*);
                        b32 padding_is_negative = false;
                        i32 padding = 0;

                        if( *at == ',' ) {
                            ++at;
                            if( *at == '-' ) {
                                ++at;
                                padding_is_negative = true;
                            }
                            ASSERT( char_is_digit( *at ) );
                            padding = parse_i32_internal( &at );
                        }
                        i32 str_len = (i32)cstr_len( str );
                        if( padding && !padding_is_negative ) {
                            padding = padding - str_len;
                            for( i32 i = 0; i < padding; ++i ) {
                                write_char( &dst, ' ' );
                            }
                        }

                        for( char* src = str; *src; ++src ) {
                            write_char( &dst, *src );
                        }
                        if( padding_is_negative && padding ) {
                            padding = padding - str_len;
                            for( i32 i = 0; i < padding; ++i ) {
                                write_char(&dst, ' ');
                            }
                        }
                        CHECK_FOR_CLOSING_BRACE();
                    } else {
                        char character = (char)va_arg( list, int );
                        write_char( &dst, character );

                        CHECK_FOR_CLOSING_BRACE();
                    }
                } break;
                case 'S':
                case 's': {
                    const char* string_buffer = NULL;
                    u32         string_length = 0;
                    ++at;

                    StringSlice arg = va_arg(list, StringSlice);
                    string_buffer = arg.buffer;
                    string_length = arg.len;

                    b32 padding_is_negative = false;
                    i32 padding = 0;

                    if( *at == ',' ) {
                        ++at;
                        if( *at == '-' ) {
                            padding_is_negative = true;
                            ++at;
                        }
                        ASSERT( char_is_digit( *at ) );
                        padding = parse_i32_internal( &at );
                    }
                    if( padding && !padding_is_negative ) {
                        padding = padding - string_length;
                        for( i32 i = 0; i < padding; ++i ) {
                            write_char( &dst, ' ' );
                        }
                    }

                    for( u32 i = 0; i < string_length; ++i ) {
                        if( string_buffer[i] ) {
                            write_char( &dst, string_buffer[i] );
                        }
                    }

                    if( padding && padding_is_negative ) {
                        padding = padding - string_length;
                        for( i32 i = 0; i < padding; ++i ) {
                            write_char( &dst, ' ' );
                        }
                    }
                    CHECK_FOR_CLOSING_BRACE();
                } break;
                case 'I':
                case 'i':
                    is_unsigned = false;
                case 'U':
                case 'u': {
                    ++at;
                    b32 value_is_64bit = false;
                    u32 size = 32;
                    if( *at == 'v' || *at == 'V' ) {
                        ++at;
                        ASSERT( char_is_digit( *at ) );
                        i32 parsed_count = parse_i32_internal( &at );
                        ASSERT( parsed_count >= 2 && parsed_count <= 4 );
                        vector_count = (u32)parsed_count;
                    } else {
                        if( char_is_digit( *at ) ) {
                            i32 parsed_size = parse_i32_internal( &at );
                            switch( parsed_size ) {
                                case 8:
                                    size = 8;
                                    break;
                                case 16:
                                    size = 16;
                                    break;
                                case 32:
                                    size = 32;
                                    break;
                                case 64:
                                    value_is_64bit = true;
                                    size = 64;
                                    break;
                                default:
                                    PANIC();
                            }
                        } else if( !(*at == '}' || *at == ',') ) {
                            PANIC();
                        }
                    }

                    b32 format_hex    = false;
                    b32 format_binary = false;
                    b32 format_zero_padding = false;
                    b32 padding_is_negative = false;
                    i32 padding = 0;
                    while( *at == ',' ) {
                        ++at;
                        if( *at == ',' ) {
                            continue;
                        }
                        if( *at == '}' ) {
                            break;
                        }

                        if( *at == '-' ) {
                            ++at;
                            padding_is_negative = true;
                        }

                        if( *at == '0' ) {
                            ++at;
                            format_zero_padding = true;
                        }

                        if( char_is_digit( *at ) ) {
                            padding = parse_i32_internal( &at );
                            continue;
                        } 

                        switch( *at ) {
                            case 'B':
                            case 'b':
                                ++at;
                                format_binary = true;
                                break;
                            case 'X':
                            case 'x':
                                ++at;
                                format_hex = true;
                                break;

                            default:
                                PANIC();
                        }
                    }

                    if( format_binary && format_hex ) {
                        continue;
                    }
                    if( format_zero_padding && format_hex ) {
                        format_zero_padding = false;
                    }

                    if( vector_count ) {
                        write_char( &dst, '{' );
                        write_char( &dst, ' ' );
                    }

                    b32 is_negative = false;
                    u64 number = 0;
                    i64 numbers[3];
                    if( !vector_count ) {
                        if( value_is_64bit ) {
                            number = va_arg( list, u64 );
                            if( !is_unsigned ) {
                                i64 signed_ = *(i64*)&number;
                                if( signed_ < 0 ) {
                                    is_negative = true;
                                    signed_ = -signed_;
                                }
                                number = signed_;
                            }
                        } else {
                            u32 number32 = va_arg( list, u32 );
                            if( !is_unsigned ) {
                                i32 signed_ = *(i32*)&number32;
                                if( signed_ < 0 ) {
                                    is_negative = true;
                                    signed_ = -signed_;
                                }
                                number32 = signed_;
                            }
                            number = (u64)number32;
                        }
                    } else {
                        i64 signed_number = 0;
                        if( vector_count == 2 ) {
                            ivec2 v = va_arg( list, ivec2 );
                            signed_number = v.x;
                            numbers[0] = v.y;
                        } else if( vector_count == 3 ) {
                            ivec3 v = va_arg( list, ivec3 );
                            signed_number = v.x;
                            numbers[0] = v.y;
                            numbers[1] = v.z;
                        } else if( vector_count == 4 ) {
                            ivec4 v = va_arg( list, ivec4 );
                            signed_number = v.x;
                            numbers[0] = v.y;
                            numbers[1] = v.z;
                            numbers[2] = v.w;
                        } 
                        if( signed_number < 0 ) {
                            is_negative = true;
                            signed_number = -signed_number;
                        }
                        number = (u64)signed_number;
                    }

                    u32 vector_index = 0;
                    do {
                        if( format_binary ) {
                            u32 temp_buffer_index = 0;
                            u32 max_len = is_unsigned ? size : size - 1;
                            for( u32 i = 0; i < max_len; ++i ) {
                                char binary = (number & (1 << i)) != 0;
                                binary = binary ? '1' : '0';
                                temp_buffer[temp_buffer_index++] = binary;
                            }
                            if( !is_unsigned ) {
                                temp_buffer[temp_buffer_index++] =
                                    is_negative ? '1' : '0';
                            }
                            if( !padding_is_negative && padding ) {
                                padding = padding - temp_buffer_index;
                                for( i32 i = 0; i < padding; ++i ) {
                                    temp_buffer[temp_buffer_index++] =
                                        format_zero_padding ? '0' : ' ';
                                }
                            }
                            for( i32 i = temp_buffer_index - 1; i >= 0; --i ) {
                                write_char( &dst, temp_buffer[i] );
                            }
                            if( padding_is_negative && padding ) {
                                padding = padding - temp_buffer_index;
                                for( i32 i = 0; i < padding; ++i ) {
                                    write_char(&dst, format_zero_padding ? '0' : ' ');
                                }
                            }
                        } else {
                            if( is_negative && !format_hex ) {
                                write_char( &dst, '-' );
                            }
                            u32 temp_buffer_index = int_to_string(
                                temp_buffer_view,
                                number,
                                format_hex ? HEX_BASE : DECIMAL_BASE,
                                format_hex ? HEX_DIGITS : DECIMAL_DIGITS,
                                padding,
                                padding_is_negative,
                                format_zero_padding,
                                is_negative
                            );
                            for( i32 i = temp_buffer_index - 1; i >= 0; --i ) {
                                write_char( &dst, temp_buffer[i] );
                            }
                            if( padding_is_negative && padding ) {
                                padding = padding - temp_buffer_index;
                                for( i32 i = 0; i < padding; ++i ) {
                                    write_char(&dst, format_zero_padding ? '0' : ' ');
                                }
                            }
                        }
                        if( vector_count ) {
                            i32 next = numbers[vector_index];
                            is_negative = next < 0;
                            if( is_negative ) {
                                next = -next;
                            }
                            number = (u64)next;

                            vector_index++;
                            if( vector_index < vector_count ) {
                                write_char( &dst, ',' );
                            }
                            write_char( &dst, ' ' );
                        }
                    } while( vector_index < vector_count );

                    if( vector_count ) {
                        write_char( &dst, '}' );
                    }

                    CHECK_FOR_CLOSING_BRACE();
                } break;
                case 'Q':
                case 'q':
                    is_quaternion = true;
                case 'V':
                case 'v':
                    vector_count = 1;
                case 'F':
                case 'f': {
                    ++at;
                    if( vector_count ) {
                        if( !is_quaternion ) {
                            ASSERT( char_is_digit( *at ) );
                            i32 parsed_vector_count =
                                parse_i32_internal( &at );
                            ASSERT(
                                parsed_vector_count == 2 ||
                                parsed_vector_count == 3 ||
                                parsed_vector_count == 4
                            );

                            vector_count = (u32)parsed_vector_count;
                        } else {
                            vector_count = 4;
                        }
                    }
                    b32 use_zero_padding = false;
                    b32 padding_is_negative = false;
                    i32 padding   = 0;
                    u32 precision = 6;
                    b32 byte_format = false;
                    u32 byte_formatter = FLOAT_BYTE_BYTES;
                    while( *at == ',' ) {
                        ++at;
                        if( *at == 'b' || *at == 'B' ) {
                            byte_format = true;
                            ++at;
                            continue;
                        }
                        if( *at == '0' ) {
                            ++at;
                            use_zero_padding = true;
                        }
                        if( *at == '-' ) {
                            padding_is_negative = true;
                            ++at;
                        }
                        ASSERT( char_is_digit( *at ) || *at == '.' );
                        padding = parse_i32_internal( &at );
                        if( *at == '.' ) {
                            ++at;
                            ASSERT( char_is_digit( *at ) );
                            precision = parse_i32_internal( &at );
                        }
                    }
                    padding *= padding_is_negative ? -1 : 1;
                    if( !vector_count ) {
                        f64 f = va_arg( list, f64 );
                        if( byte_format ) {
                            FloatByteFormatResult format =
                                float_byte_format( f );
                            f = format.f;
                            byte_formatter = format.format;
                        }
                        u32 write_count = float_to_string(
                            temp_buffer_view,
                            f,
                            padding,
                            precision,
                            use_zero_padding
                        );
                        for( i32 i = write_count - 1; i >= 0; --i ) {
                            write_char( &dst, temp_buffer[i] );
                        }
                        if( byte_format ) {
                            write_char(&dst, ' ');
                            switch( byte_formatter ) {
                                case FLOAT_BYTE_BYTES:
                                    write_char( &dst, ' ' );
                                    break;
                                case FLOAT_BYTE_KILO:
                                    write_char(&dst, 'K');
                                    break;
                                case FLOAT_BYTE_MEGA:
                                    write_char(&dst, 'M');
                                    break;
                                case FLOAT_BYTE_GIGA:
                                    write_char(&dst, 'G');
                                    break;
                            }
                            write_char(&dst, 'B');
                            write_count += 3;
                        }
                        if( padding_is_negative ) {
                            padding = -padding - (write_count - precision);
                            for( i32 i = 0; i < padding; ++i ) {
                                write_char( &dst, use_zero_padding ? '0' : ' ' );
                            }
                        }
                    } else {
                        write_char( &dst, '{' );
                        write_char( &dst, ' ' );

                        f32 values[4];
                        if( is_quaternion ) {
                            quat q = va_arg(list, quat);
                            values[0] = q.w;
                            values[1] = q.x;
                            values[2] = q.y;
                            values[3] = q.z;
                        } else {
                            if( vector_count == 2 ) {
                                vec2 v = va_arg( list, vec2 );
                                values[0] = v.x;
                                values[1] = v.y;
                            } else if( vector_count == 3 ) {
                                vec3 v = va_arg( list, vec3 );
                                values[0] = v.x;
                                values[1] = v.y;
                                values[2] = v.z;
                            } else if( vector_count == 4 ) {
                                vec4 v = va_arg( list, vec4 );
                                values[0] = v.x;
                                values[1] = v.y;
                                values[2] = v.z;
                                values[3] = v.w;
                            } else {
                                PANIC();
                            }
                        }

                        for( u32 i = 0; i < vector_count; ++i ) {
                            u32 write_count = float_to_string(
                                temp_buffer_view,
                                values[i],
                                padding,
                                precision,
                                use_zero_padding
                            );
                            for( i32 i = write_count - 1; i >= 0; --i ) {
                                write_char( &dst, temp_buffer[i] );
                            }
                            if( i + 1 != vector_count ) {
                                write_char( &dst, ',' );
                            }
                            write_char( &dst, ' ' );
                        }
                        write_char( &dst, '}' );
                    }

                    CHECK_FOR_CLOSING_BRACE();
                } break;
                case '}': {
                } break;

                default:
                    PANIC();
            }

            if( *at ) {
                ++at;
            }
        } else {
            write_char( &dst, *at++ );
        }
    }

    u32 result = dst.buffer - buffer.buffer;
    return result;
}

LD_API usize ss_mut_format_va(
    StringSlice* buffer,
    const char* format,
    va_list variadic
) {
    return format_internal( *buffer, format, write_char_dst, variadic );
}
LD_API usize ss_mut_format( StringSlice* buffer, const char* format, ... ) {
    va_list list;
    va_start( list, format );
    usize result = format_internal( *buffer, format, write_char_dst, list );
    va_end( list );
    return result;
}

LD_API void print( const char* format, ... ) {
    StringSlice buffer = {};
    buffer.len = U32_MAX;
    va_list list;
    va_start( list, format );
    format_internal( buffer, format, write_char_stdout, list );
    va_end( list );
    char_output_stderr(0);
}
LD_API void print_err( const char* format, ... ) {
    StringSlice buffer = {};
    buffer.len = U32_MAX;
    va_list list;
    va_start( list, format );
    format_internal( buffer, format, write_char_stderr, list );
    va_end( list );
    char_output_stderr(0);
}
LD_API void print_va( const char* format, va_list variadic ) {
    StringSlice buffer = {};
    buffer.len = U32_MAX;
    format_internal(
        buffer,
        format,
        write_char_stdout,
        variadic
    );
    char_output_stderr(0);
}
LD_API void print_err_va( const char* format, va_list variadic ) {
    StringSlice buffer = {};
    buffer.len = U32_MAX;
    format_internal(
        buffer,
        format,
        write_char_stderr,
        variadic
    );
    char_output_stderr(0);
}

