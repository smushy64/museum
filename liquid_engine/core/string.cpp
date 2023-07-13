/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "string.h"
#include "logging.h"
#include "memory.h"

#include "math.h"

#include "platform/platform.h"

inline internal b32 dstring_allocate( u32 capacity, String* out_string ) {
    void* buffer = mem_alloc( capacity, MEMTYPE_STRING );
    if( !buffer ) {
        return false;
    }
    out_string->buffer   = (char*)buffer;
    out_string->capacity = capacity;
    return true;
}
inline internal b32 dstring_reallocate( String* string, u32 new_capacity ) {
    void* new_buffer = mem_realloc( string->buffer, new_capacity );
    if( !new_buffer ) {
        return false;
    }
    string->buffer   = (char*)new_buffer;
    string->capacity = new_capacity;
    return true;
}
inline internal b32 dstring_append_internal(
    String* string,
    u32 append_len, const char* append_buffer,
    b32 alloc
) {
    u32 new_len = string->len + append_len;
    u32 max_len = new_len > string->capacity ? string->capacity : new_len;
    if( alloc ) {
        if( string->capacity < new_len ) {
            if( !dstring_reallocate( string, new_len ) ) {
                return false;
            }
            max_len = new_len;
        }
    }

    mem_copy( string->buffer + string->len, append_buffer, max_len );
    string->len = max_len;

    return true;
}

LD_API b32 dstring_new( StringView view, String* out_string ) {
    u32 capacity = view.len;
    if( !dstring_allocate( capacity, out_string ) ) {
        return false;
    }
    out_string->len = capacity;
    mem_copy( out_string->buffer, view.buffer, view.len );

    return true;
}
LD_API b32 dstring_with_capacity( u32 capacity, String* out_string ) {
    out_string->len = 0;
    return dstring_allocate( capacity, out_string );
}
LD_API b32 dstring_reserve( String* string, u32 new_capacity ) {
    return dstring_reallocate( string, new_capacity );
}
LD_API b32 dstring_append( String* string, String* append, b32 alloc ) {
    return dstring_append_internal(
        string,
        append->len,
        append->buffer,
        alloc
    );
}
LD_API b32 dstring_append( String* string, StringView append, b32 alloc ) {
    return dstring_append_internal(
        string,
        append.len,
        append.buffer,
        alloc
    );
}
LD_API void dstring_free( String* string ) {
    if( string->buffer ) {
        mem_free( string->buffer );
    }
    *string = {};
}

inline internal b32 string_cmp_internal(
    u32 a_len, const char* a_buffer,
    u32 b_len, const char* b_buffer
) {
    if( a_len != b_len ) {
        return false;
    }

    // TODO(alicia): traverse by u64 instead
    for( u32 i = 0; i < a_len; ++i ) {
        if( a_buffer[i] != b_buffer[i] ) {
            return false;
        }
    }

    return true;
}

LD_API b32 string_cmp( String* a, String* b ) {
    return string_cmp_internal(
        a->len, a->buffer,
        b->len, b->buffer
    );
}
LD_API b32 string_cmp( String* a, StringView  b ) {
    return string_cmp_internal(
        a->len, a->buffer,
        b.len, b.buffer
    );
}
LD_API b32 string_cmp( StringView  a, String* b ) {
    return string_cmp_internal(
        a.len, a.buffer,
        b->len, b->buffer
    );
}
LD_API b32 string_cmp( StringView  a, StringView  b ) {
    return string_cmp_internal(
        a.len, a.buffer,
        b.len, b.buffer
    );
}

inline internal void string_copy_internal(
    u32 src_len, const char* src_buffer,
    u32 dst_capacity, char* dst_buffer
) {
    u32 max_len = src_len > dst_capacity ? dst_capacity : src_len;
    mem_copy( dst_buffer, src_buffer, max_len );
}
LD_API void string_copy( String* src, String* dst ) {
    string_copy_internal(
        src->len, src->buffer,
        dst->capacity, dst->buffer
    );
    if( src->len > dst->len ) {
        dst->len = dst->capacity;
    }
}
LD_API void string_copy( StringView src, String* dst ) {
    string_copy_internal(
        src.len, src.buffer,
        dst->capacity, dst->buffer
    );
    if( src.len > dst->len ) {
        dst->len = dst->capacity;
    }
}
LD_API void string_trim_trailing_whitespace( String* string ) {
    for( u32 i = 0; i < string->len; ++i ) {
        if( char_is_whitespace( string->buffer[i] ) ) {
            string->len = i;
            return;
        }
    }
}
LD_API void string_trim_trailing_whitespace( StringView string_view ) {
    for( u32 i = 0; i < string_view.len; ++i ) {
        if( char_is_whitespace( string_view.buffer[i] ) ) {
            string_view.len = i;
            return;
        }
    }
}
LD_API b32 dstring_push_char( String* string, char character, u32 realloc ) {
    if( string->len == string->capacity ) {
        if( realloc ) {
            if( !dstring_reserve( string, string->capacity + realloc ) ) {
                return false;
            }
        } else {
            return true;
        }
    }

    string->buffer[string->len++] = character;
    return true;
}
LD_API char dstring_pop_char( String* string ) {
    if( !string->len ) {
        return 0;
    }
    return string->buffer[string->len--];
}
LD_API b32 string_contains( StringView string_view, StringView phrase ) {
    if( string_view.len < phrase.len ) {
        return false;
    }
   
    u32 i = 0;
    do {
        u32 remaining_len = string_view.len - i;
        if( remaining_len < phrase.len ) {
            return false;
        }
        StringView window = {};
        window.buffer = &string_view.buffer[i];
        window.len    = phrase.len;
        if( string_cmp( window, phrase ) ) {
            return true;
        }
        ++i;
    } while( i < string_view.len );

    return false;
}
LD_API b32 str_view(
    u32 str_len,
    const char* str,
    u32 offset, u32 len,
    StringView* out_view
) {
    if( offset >= str_len ) {
        return false;
    }
    u32 view_end = offset + len;
    if( view_end >= str_len ) {
        return false;
    }

    out_view->buffer = (char*)(str + offset);
    out_view->len    = len;

    return true;
}

LD_API StringView dstring_view_capacity_bounds(
    String* string, u32 offset
) {
    StringView result = {};
    if( offset >= string->capacity ) {
        LOG_FATAL("Attempted to create dstring view with invalid offset!");
        PANIC();
    }
    result.buffer = string->buffer + offset;
    result.len    = string->capacity;
    return result;
}
LD_API StringView dstring_view_len_bounds(
    String* string,
    u32 offset
) {
    StringView result = {};
    if( offset >= string->len ) {
        LOG_FATAL("Attempted to create dstring view with invalid offset!");
        PANIC();
    }
    result.buffer = string->buffer + offset;
    result.len    = string->len - offset;
    return result;
}
LD_API b32 dstring_view_writeable(
    String* string,
    u32 offset,
    StringView* out_view
);
LD_API usize str_length( const char* string ) {
    usize result = 0;

    if( string ) {
        while( *string++ ) {
            result++;
        }
    }

    return result;
}

LD_API void str_buffer_fill(
    u32 buffer_size,
    char* buffer,
    char character
) {
    for( u32 i = 0; i < buffer_size; ++i ) {
        buffer[i] = character;
    }
    buffer[buffer_size - 1] = 0;
}

internal inline i32 parse_i32_internal( char** at_init ) {
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
LD_API i32 string_parse_i32( StringView s ) {
    i32 result = parse_i32_internal( (char**)&s.buffer );
    return result;
}
LD_API u32 string_parse_u32( StringView s ) {
    i32 result = parse_i32_internal( (char**)&s.buffer );
    return *(u32*)&result;
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
internal FORCE_INLINE u32 to_string(
    StringView view,
    u64 value,
    u64 base,
    char* digits,
    u32 padding,
    b32 padding_is_negative,
    b32 use_zero_padding,
    b32 is_negative
) {
    if( !view.len ) {
        return 0;
    }
    u32 view_index = 0;

    if( value == 0 ) {
        view.buffer[view_index++] = digits[0];
    }

    while( view_index < view.len && value ) {
        u64 digit = value % base;
        view.buffer[view_index++] = digits[digit];
        value /= base;
    }
    if( base == HEX_BASE && (view_index + 2) < view.len ) {
        view.buffer[view_index++] = 'x';
        view.buffer[view_index++] = '0';
    }

    if( !padding_is_negative && padding && padding >= view_index ) {
        padding = padding - view_index;
        if( padding ) {
            padding -= is_negative;
        }
        u32 remaining_len = view.len - view_index;
        u32 max_len = padding > remaining_len ? remaining_len : padding;
        for( u32 i = 0; i < max_len; ++i ) {
            view.buffer[view_index++] =
                use_zero_padding ? '0' : ' ';
        }
    }
    return view_index;
}

#define FLOAT_BYTE_BYTES 0
#define FLOAT_BYTE_KILO  1
#define FLOAT_BYTE_MEGA  2
#define FLOAT_BYTE_GIGA  3
struct FloatByteFormatResult {
    u32 format;
    f64 f;
};
internal inline FloatByteFormatResult float_byte_format( f64 f ) {
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

internal inline u32 to_string(
    StringView view,
    f64 value,
    i32 padding,
    u32 precision,
    b32 use_zero_padding
) {
    if( !view.len ) {
        return 0;
    }

    if( is_nan( value ) ) {
        view.buffer[0] = 'N';
        view.buffer[1] = 'A';
        view.buffer[2] = 'N';
        return 3;
    }

    b32 padding_is_negative = padding < 0;

    u32 write_count = 0;
    u32 integer_write_count = 0;
    u32 fract_write_count   = 0;
    u32 view_index  = 0;

    b32 value_is_negative = value < 0.0;
    if( value_is_negative ) {
        value = -value;
    }

    u64 integer_part = trunc64( value );
    f64 fract_part   = value - (f64)integer_part;

    u32 max_fract_write_count = precision > view.len ? view.len - 1 : precision;
    for( u32 i = 0; i < max_fract_write_count; ++i ) {
        fract_part *= 10.0;
        u64 fract_part_int = clamp(trunc64(fract_part), 0ll, 9ll);
        fract_part -= (f64)fract_part_int;
        u32 buffer_index = (max_fract_write_count - 1) - i;
        view.buffer[buffer_index] = DECIMAL_DIGITS[fract_part_int];
        fract_write_count++;
        view_index++;
    }

    if( view_index < view.len ) {
        view.buffer[view_index++] = '.';
        write_count++;
    }

    if( !integer_part && view_index < view.len ) {
        view.buffer[view_index++] = '0';
        integer_write_count++;
    }
    while( view_index < view.len && integer_part ) {
        u64 digit = clamp((integer_part % 10ull), 0ull, 9ull);
        view.buffer[view_index++] = DECIMAL_DIGITS[digit];
        integer_part /= 10;
        integer_write_count++;
    }

    if( !use_zero_padding ) {
        if( value_is_negative && view_index < view.len ) {
            view.buffer[view_index++] = '-';
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
        u32 remaining_len = view.len - integer_write_count;
        u32 max_len = padding > (i32)remaining_len ? remaining_len : padding;
        for( u32 i = 0; i < max_len; ++i ) {
            view.buffer[view_index++] =
                use_zero_padding ? '0' : ' ';
            write_count++;
        }
    }

    if( use_zero_padding ) {
        if( value_is_negative && view_index < view.len ) {
            view.buffer[view_index++] = '-';
            write_count++;
        }
    }

    write_count += integer_write_count + fract_write_count;
    return write_count;
}

struct PODStringView {
    char* buffer;
    u32 len;
};

typedef b32 (*WriteCharFN)( struct StringView*, char );

NO_INLINE
internal b32 write_char_dst( StringView* dst, char character ) {
    if( dst->len ) {
        --dst->len;
        *dst->buffer++ = character;
        return true;
    } else {
        return false;
    }
}
NO_INLINE
internal b32 write_char_stdout( StringView*, char character ) {
    stdout_push( character );
    return true;
}
NO_INLINE
internal b32 write_char_stderr( StringView*, char character ) {
    stderr_push( character );
    return true;
}

NO_INLINE HOT_PATH
internal u32 format_internal(
    StringView buffer,
    const char* format,
    WriteCharFN write_char_fn,
    va_list list
) {
    StringView dst = buffer.clone();
    if( !dst.len ) {
        u32 result = dst.buffer - buffer.buffer;
        return result;
    }

    char* at = (char*)format;
    #define TEMP_BUFFER_SIZE 64
    char temp_buffer[TEMP_BUFFER_SIZE];
    StringView temp_buffer_view = {};
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
                        i32 str_len = (i32)str_length( str );
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
                    const char* string_buffer = nullptr;
                    u32         string_length = 0;
                    ++at;
                    if( *at == 'v' || *at == 'V' ) {
                        PODStringView arg = va_arg(list, PODStringView);
                        ++at;
                        string_buffer = arg.buffer;
                        string_length = arg.len;
                    } else {
                        String arg = va_arg( list, String );
                        string_buffer = arg.buffer;
                        string_length = arg.len;
                    }
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
                            u32 temp_buffer_index = to_string(
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
                        u32 write_count = to_string(
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
                            u32 write_count = to_string(
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

LD_API u32 string_format_va(
    StringView buffer,
    const char* format,
    va_list variadic
) {
    return format_internal( buffer, format, write_char_dst, variadic );
}

LD_API u32 string_format( StringView buffer, const char* format, ... ) {
    va_list list;
    va_start( list, format );
    u32 result = format_internal( buffer, format, write_char_dst, list );
    va_end( list );
    return result;
}
LD_API void print( const char* format, ... ) {
    StringView buffer = {};
    buffer.len = U32::MAX;
    va_list list;
    va_start( list, format );
    format_internal( buffer, format, write_char_stdout, list );
    va_end( list );
    stdout_push(0);
}
LD_API void printerr( const char* format, ... ) {
    StringView buffer = {};
    buffer.len = U32::MAX;
    va_list list;
    va_start( list, format );
    format_internal( buffer, format, write_char_stderr, list );
    va_end( list );
    stderr_push(0);
}
LD_API void print_va( const char* format, va_list variadic ) {
    StringView buffer = {};
    buffer.len = U32::MAX;
    format_internal(
        buffer,
        format,
        write_char_stdout,
        variadic
    );
    stdout_push(0);
}
LD_API void printerr_va( const char* format, va_list variadic ) {
    StringView buffer = {};
    buffer.len = U32::MAX;
    format_internal(
        buffer,
        format,
        write_char_stderr,
        variadic
    );
    stderr_push(0);
}

LD_API void stdout_push( char character ) {
    platform_write_console(
        platform_stdout_handle(),
        1, &character
    );
}
LD_API void stderr_push( char character ) {
    platform_write_console(
        platform_stderr_handle(),
        1, &character
    );
}

HOT_PATH
LD_API void output_string_stdout( const char* str ) {
    u32 str_len = (u32)str_length( str );
    platform_write_console(
        platform_stdout_handle(),
        str_len, str
    );
    stdout_push( 0 );
}

HOT_PATH
LD_API void output_string_stdout( StringView string_view ) {
    platform_write_console(
        platform_stdout_handle(),
        string_view.len, string_view.buffer
    );
    stdout_push( 0 );
}

HOT_PATH
LD_API void output_string_view_stdout( StringView string_view ) {
    platform_write_console(
        platform_stdout_handle(),
        string_view.len, string_view.buffer
    );
}

HOT_PATH
LD_API void output_string_stdout( String* string ) {
    platform_write_console(
        platform_stdout_handle(),
        string->len, string->buffer
    );
    stdout_push( 0 );
}

HOT_PATH
LD_API void output_string_stderr( const char* str ) {
    u32 str_len = (u32)str_length( str );
    platform_write_console(
        platform_stderr_handle(),
        str_len, str
    );
    stderr_push( 0 );
}

HOT_PATH
LD_API void output_string_stderr( StringView string_view ) {
    platform_write_console(
        platform_stderr_handle(),
        string_view.len, string_view.buffer
    );
    stderr_push( 0 );
}

HOT_PATH
LD_API void output_string_view_stderr( StringView string_view ) {
    platform_write_console(
        platform_stderr_handle(),
        string_view.len, string_view.buffer
    );
}

HOT_PATH
LD_API void output_string_stderr( String* string ) {
    platform_write_console(
        platform_stderr_handle(),
        string->len, string->buffer
    );
    stderr_push( 0 );
}

