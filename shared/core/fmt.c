/**
 * Description:  String formatting implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 25, 2023
*/
#include "defines.h"
#include "constants.h"
#include "core/fmt.h"

#if defined(LD_ARCH_X86) && LD_SIMD_WIDTH != 1
#include <immintrin.h>
#endif

typedef enum FMTIdentifier : u32 {
    FMT_IDENT_UNKNOWN,
    FMT_IDENT_LITERAL_PAREN,
    FMT_IDENT_BOOL,
    FMT_IDENT_CHAR,
    FMT_IDENT_CSTR,
    FMT_IDENT_FLOAT,
    FMT_IDENT_FLOAT32,
    FMT_IDENT_FLOAT64,
    FMT_IDENT_VECTOR_2,
    FMT_IDENT_VECTOR_3,
    FMT_IDENT_VECTOR_4,
    FMT_IDENT_INT,
    FMT_IDENT_INT8,
    FMT_IDENT_INT16,
    FMT_IDENT_INT32,
    FMT_IDENT_INT64,
    FMT_IDENT_INT_VECTOR_2,
    FMT_IDENT_INT_VECTOR_3,
    FMT_IDENT_INT_VECTOR_4,
    FMT_IDENT_UINT,
    FMT_IDENT_UINT8,
    FMT_IDENT_UINT16,
    FMT_IDENT_UINT32,
    FMT_IDENT_UINT64,
    FMT_IDENT_UINT_VECTOR_2,
    FMT_IDENT_UINT_VECTOR_3,
    FMT_IDENT_UINT_VECTOR_4,
    FMT_IDENT_STRING_SLICE,
} FMTIdentifier;
typedef enum : u32 {
    FMT_FORMAT_NORMAL = 0,
    FMT_FORMAT_BINARY = 128,
    FMT_FORMAT_HEXADECIMAL_LOWER,
    FMT_FORMAT_HEXADECIMAL_UPPER,
    FMT_FORMAT_MEMORY,
} FMTFormat;
internal FormatInteger ___internal_fmt_format_to_fmt_integer( FMTFormat format ) {
    switch( format ) {
        case FMT_FORMAT_NORMAL: return FORMAT_INTEGER_DECIMAL;
        case FMT_FORMAT_BINARY: return FORMAT_INTEGER_BINARY;
        case FMT_FORMAT_HEXADECIMAL_LOWER: return FORMAT_INTEGER_HEXADECIMAL_LOWER;
        case FMT_FORMAT_HEXADECIMAL_UPPER: return FORMAT_INTEGER_HEXADECIMAL_UPPER;
        default: panic();
    }
}
typedef enum : u32 {
    FMT_FORMAT_WIDTH_NORMAL,
    FMT_FORMAT_WIDTH_FULL,
    FMT_FORMAT_WIDTH_SEPARATOR
} FMTFormatWidth;
typedef enum : u32 {
    FMT_FORMAT_CASE_UPPER  = 1,
    FMT_FORMAT_CASE_NORMAL = 0,
    FMT_FORMAT_CASE_LOWER  = -1,
} FMTFormatCase;

struct FMTIdentifierArguments {
    u32 count; // 0 = no pointer, 1 = pointer
    union {
        FMTFormat format;
        b32       is_binary;
    };
    FMTFormatWidth width;
    u32 precision;
    union {
        b32           zero_padding;
        FMTFormatCase casing;
    };
    i64 padding;
};
union FMTInteger {
    u8  u8;
    u16 u16;
    u32 u32;
    u64 u64;
    i8  i8;
    i16 i16;
    i32 i32;
    i64 i64;
};

internal b32 ___find_char(
    usize len, const char* cstr,
    char character, usize* out_index ); 

internal
usize ___internal_fmt_integer(
    FormatWriteFN* write, void* target,
    union FMTInteger integer, FormatInteger format,
    b32 is_signed, u32 size, FMTFormatWidth width );

internal
usize ___internal_fmt_float(
    FormatWriteFN* write, void* target, f64 f,
    u32 precision, FMTFormatWidth width );

internal b32 ___char_is_number( char c ) {
    return c >= '0' && c <= '9';
}
internal char ___char_to_upper( char c ) {
    if( c >= 'a' && c <= 'z' ) {
        return c - ('a' - 'A');
    }
    return c;
}
internal char ___char_to_lower( char c ) {
    if( c >= 'A' && c <= 'Z' ) {
        return c + ('a' - 'A');
    }
    return c;
}
internal usize ___strlen( const char* c ) {
    usize result = 0;
    while( *c++ ) { result++; }
    return result;
}
typedef enum {
    FMT_STORAGE_BYTES,
    FMT_STORAGE_KB   ,
    FMT_STORAGE_MB   ,
    FMT_STORAGE_GB   ,
    FMT_STORAGE_TB   ,
} FMTStorage;
maybe_unused
internal f64 ___determine_storage( f64 f, FMTStorage* out_storage ) {
    f64 result = f;
    *out_storage = FMT_STORAGE_BYTES;
    if( result >= 1024.0 ) {
        *out_storage = FMT_STORAGE_KB;
        result /= 1024.0;
        if( result >= 1024.0 ) {
            *out_storage = FMT_STORAGE_MB;
            result /= 1024.0;
            if( result >= 1024.0 ) {
                *out_storage = FMT_STORAGE_GB;
                result /= 1024.0;
                if( result >= 1024.0 ) {
                    *out_storage = FMT_STORAGE_TB;
                    result /= 1024.0;
                }
            }
        }
    }
    return result;
}

internal b32 ___collect_argument(
    usize remaining, char* at,
    usize* out_argument_len
) {
    usize argument_len = 0;
    for( usize i = 0; i < remaining; ++i ) {
        if( at[i] == ',' || at[i] == '}' ) {
            *out_argument_len = argument_len;
            return argument_len;
        }
        argument_len++;
    }

    return false;
}

#define _advance() do {\
    *remaining = *remaining - 1;\
    if( !(*remaining) ) {\
        *in_at = at;\
        return 0;\
    }\
    at++;\
} while(0)

#define _advance_by(count) do {\
    if( count > *remaining ) {\
        *in_at = at;\
        return 0;\
    }\
    *remaining = *remaining - count;\
    if( !(*remaining) ) {\
        *in_at = at;\
        return 0;\
    }\
    at += count;\
} while(0)
internal FMTIdentifier ___determine_identifier(
    usize* remaining, char** in_at
) {
    char* at = *in_at;

    #define check_int( type, ptr_size )\
    _advance();\
    identifier = FMT_IDENT_##type;\
    switch( *at ) {\
        case '8': {\
            _advance();\
            identifier = FMT_IDENT_##type##8;\
        } break;\
        case '1': {\
            _advance();\
            if( *at == '6' ) {\
                _advance();\
                identifier = FMT_IDENT_##type##16;\
            } else {\
                identifier = FMT_IDENT_UNKNOWN;\
            }\
        } break;\
        case '3': {\
            _advance();\
            if( *at == '2' ) {\
                _advance();\
                identifier = FMT_IDENT_##type##32;\
            } else {\
                identifier = FMT_IDENT_UNKNOWN;\
            }\
        } break;\
        case '6': {\
            _advance();\
            if( *at == '4' ) {\
                _advance();\
                identifier = FMT_IDENT_##type##64;\
            } else {\
                identifier = FMT_IDENT_UNKNOWN;\
            }\
        } break;\
        case 's': {\
            _advance();\
            b32 valid = false;\
            if( *at == 'i' ) {\
                _advance();\
                if( *at == 'z' ) {\
                    _advance();\
                    if( *at == 'e' ) {\
                        _advance();\
                        valid = true;\
                    }\
                }\
            }\
            if( valid ) {\
                identifier = FMT_IDENT_##type##ptr_size;\
            } else {\
                identifier = FMT_IDENT_UNKNOWN;\
            }\
        } break;\
        case 'v': {\
            _advance();\
            switch( *at ) {\
                case '2': {\
                    _advance();\
                    identifier = FMT_IDENT_##type##_VECTOR_2;\
                } break;\
                case '3': {\
                    _advance();\
                    identifier = FMT_IDENT_##type##_VECTOR_3;\
                } break;\
                case '4': {\
                    _advance();\
                    identifier = FMT_IDENT_##type##_VECTOR_4;\
                } break;\
            }\
        } break;\
    }\


    FMTIdentifier identifier = FMT_IDENT_UNKNOWN;
    switch( *at ) {
        case '{': {
            _advance();
            *in_at = at;
            return identifier = FMT_IDENT_LITERAL_PAREN;
        } break;
        case 'c': {
            _advance();
            identifier = FMT_IDENT_CHAR;
            if( *at == 'c' ) {
                _advance();
                identifier = FMT_IDENT_CSTR;
            }
        } break;
        case 'b': {
            _advance();
            identifier = FMT_IDENT_BOOL;
        } break;
        case 'f': {
            _advance();
            identifier = FMT_IDENT_FLOAT;
            switch( *at ) {
                case '3': {
                    _advance();
                    if( *at == '2' ) {
                        _advance();
                        identifier = FMT_IDENT_FLOAT32;
                    } else {
                        identifier = FMT_IDENT_UNKNOWN;
                    }
                } break;
                case '6': {
                    _advance();
                    if( *at == '4' ) {
                        _advance();
                        identifier = FMT_IDENT_FLOAT64;
                    } else {
                        identifier = FMT_IDENT_UNKNOWN;
                    }
                } break;
            }
        } break;
        case 'v': {
            _advance();
            switch( *at ) {
                case '2': {
                    _advance();
                    identifier = FMT_IDENT_VECTOR_2;
                } break;
                case '3': {
                    _advance();
                    identifier = FMT_IDENT_VECTOR_3;
                } break;
                case '4': {
                    _advance();
                    identifier = FMT_IDENT_VECTOR_4;
                } break;
                default: {
                    identifier = FMT_IDENT_UNKNOWN;
                } break;
            }
        } break;
        case 'i': {
#if defined(LD_ARCH_64_BIT)
            check_int( INT, 64 );
#else
            check_int( INT, 32 );
#endif
        } break;
        case 'u': {
#if defined(LD_ARCH_64_BIT)
            check_int( UINT, 64 );
#else
            check_int( UINT, 32 );
#endif
        } break;
        case 's': {
            _advance();
            identifier = FMT_IDENT_STRING_SLICE;
        } break;
    }

    if( !(*at == ',' || *at == '}') ) {
        identifier = FMT_IDENT_UNKNOWN;
    }
    *in_at = at;
    return identifier;
}
internal b32 ___process_arguments(
    usize* remaining, char** in_at,
    FMTIdentifier identifier,
    struct FMTIdentifierArguments* out_args
) {
    struct FMTIdentifierArguments args = {};
    char* at = *in_at;

    #define failed()\
        *in_at = at;\
        return false

    switch( identifier ) {
        case FMT_IDENT_FLOAT ... FMT_IDENT_VECTOR_4: {
            args.precision = 6;
        } break;
        default: break;
    }

    while( *at == ',' ) {
        _advance();

        switch( *at ) {
            // binary
            case 'b': {
                switch( identifier ) {
                    case FMT_IDENT_BOOL: {
                        args.is_binary = true;
                    } break;
                    case FMT_IDENT_CHAR ... FMT_IDENT_VECTOR_4:
                    case FMT_IDENT_STRING_SLICE: {
                        failed();
                    } break;
                    default: {
                        args.format = FMT_FORMAT_BINARY;
                    } break;
                }
                _advance();
                continue;
            } break;
            // hex upper
            case 'X': {
                switch( identifier ) {
                    case FMT_IDENT_INT ... FMT_IDENT_UINT_VECTOR_4: {
                        args.format = FMT_FORMAT_HEXADECIMAL_UPPER;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
            // hex lower
            case 'x': {
                switch( identifier ) {
                    case FMT_IDENT_INT ... FMT_IDENT_UINT_VECTOR_4: {
                        args.format = FMT_FORMAT_HEXADECIMAL_LOWER;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
            // full width
            case 'f': {
                switch( identifier ) {
                    case FMT_IDENT_INT ... FMT_IDENT_UINT_VECTOR_4: {
                        args.width = FMT_FORMAT_WIDTH_FULL;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
            // with separators
            case 's': {
                switch( identifier ) {
                    case FMT_IDENT_FLOAT ... FMT_IDENT_UINT_VECTOR_4: {
                        args.width = FMT_FORMAT_WIDTH_SEPARATOR;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
            // upper case
            case 'u': {
                switch( identifier ) {
                    case FMT_IDENT_CHAR:
                    case FMT_IDENT_CSTR:
                    case FMT_IDENT_STRING_SLICE: {
                        args.casing = FMT_FORMAT_CASE_UPPER;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
            // lower case
            case 'l': {
                switch( identifier ) {
                    case FMT_IDENT_CHAR:
                    case FMT_IDENT_CSTR:
                    case FMT_IDENT_STRING_SLICE: {
                        args.casing = FMT_FORMAT_CASE_LOWER;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
            // count
            case '*': {
                _advance();
                if( ___char_is_number( *at ) ) {
                    usize argument_len = 0;
                    if( ___collect_argument(
                        *remaining, at, &argument_len
                    ) ) {
                        u64 parsed_int = 0;
                        if( !fmt_read_uint(
                            argument_len, at, &parsed_int
                        ) ) {
                            failed();
                        }
                        args.count = parsed_int;
                        _advance_by( argument_len );
                    } else {
                        failed();
                    }
                } else {
                    args.count = 1;
                }
                continue;
            } break;
            // memory formatting
            case 'm': {
                switch( identifier ) {
                    case FMT_IDENT_FLOAT:
                    case FMT_IDENT_UINT ... FMT_IDENT_UINT64: {
                        args.format = FMT_FORMAT_MEMORY;
                    } break;
                    default: {
                        failed();
                    } break;
                }
                _advance();
                continue;
            } break;
        }

        // padding
        usize argument_len = 0;
        if( ___collect_argument( *remaining, at, &argument_len ) ) {
            usize dot_position = 0;
            if( ___find_char( argument_len, at, '.', &dot_position ) ) {
                switch( identifier ) {
                    case FMT_IDENT_FLOAT ... FMT_IDENT_VECTOR_4: break;
                    default: failed();
                }

                usize left_side_len  = dot_position;
                usize right_side_len = argument_len - dot_position - 1;
                char* left_side      = at;
                char* right_side     = at + left_side_len + 1;

                if( left_side_len ) {
                    if( !fmt_read_int( left_side_len, left_side, &args.padding ) ) {
                        failed();
                    }
                    switch( identifier ) {
                        case FMT_IDENT_CHAR:
                        case FMT_IDENT_CSTR:
                        case FMT_IDENT_STRING_SLICE: break;
                        default: {
                            if( at[0] == '0' ) {
                                args.zero_padding = true;
                            }
                        } break;
                    }
                }

                u64 precision = 0;
                if( !fmt_read_uint( right_side_len, right_side, &precision ) ) {
                    failed();
                }
                args.precision = precision;
            } else {
                if( !fmt_read_int( argument_len, at, &args.padding ) ) {
                    failed();
                }
                switch( identifier ) {
                    case FMT_IDENT_CHAR:
                    case FMT_IDENT_CSTR:
                    case FMT_IDENT_STRING_SLICE: break;
                    default: {
                        if( at[0] == '0' ) {
                            args.zero_padding = true;
                        }
                    } break;
                }
            }
            _advance_by( argument_len );
        } else {
            failed();
        }
    }

    if( args.format != FMT_FORMAT_NORMAL && args.zero_padding ) {
        args.zero_padding = false;
    }

    if( args.format == FMT_FORMAT_MEMORY && args.count > 1 ) {
        failed();
    }

    *out_args = args;
    *in_at    = at;
    return true;
}

#define FMT_INTERMEDIATE_BUFFER_SIZE (128)
struct FMTIntermediate { char buffer[FMT_INTERMEDIATE_BUFFER_SIZE]; usize len; };
usize ___write_intermediate( void* target, usize len, char* string ) {
    struct FMTIntermediate* intermediate = target;

    usize result = 0;
    for( usize i = 0; i < len; ++i ) {
        if( intermediate->len == FMT_INTERMEDIATE_BUFFER_SIZE ) {
            result++;
            continue;
        }
        intermediate->buffer[intermediate->len++] = string[i];
    }
    return result;
}

LD_API usize ___internal_fmt_write_va(
    FormatWriteFN* write, void* target,
    usize format_len, const char* format, va_list va
) {
    struct FMTIntermediate intermediate = {};

    #define intermediate_push( character )\
        assert( intermediate_len + 1 <= FMT_INTERMEDIATE_BUFFER_SIZE );\
        intermediate_buffer[intermediate_len++] = character

    usize remaining = format_len;
    char* at = (char*)format;

    usize result = 0;

    #define write_string( len, string )\
        result += write( target, len, string )
    #define write_string_literal( literal )\
        result += write( target, sizeof(literal)-1, literal )
    #define write_char( character ) do {\
        char tmp = character;\
        result += write( target, 1, &tmp );\
    } while(0)
    #define write_string_padded( c, len, string )\
        if( args.padding > 0 ) {\
            apply_padding( c, args.padding, len );\
        }\
        write_string( len, string );\
        if( args.padding < 0 ) {\
            apply_padding( c, args.padding, len );\
        }\

    #define advance() do {\
        remaining--;\
        if( !remaining ) {\
            goto fmt_end;\
        }\
        at++;\
    } while(0)
    #define advance_by( number ) do {\
        if( number > remaining ) {\
            goto fmt_end;\
        }\
        remaining -= number;\
        if( !remaining ) {\
            goto fmt_end;\
        }\
        at += number;\
    } while(0)
    #define check_end() do {\
        if( *at != '}' ) {\
            goto fmt_end;\
        }\
    } while(0)
    #define apply_padding( pad_char, padding, arglen ) do {\
        if( padding < 0 ) { padding = -padding; }\
        isize pad_count = padding - arglen;\
        for( isize i = 0; i < pad_count; ++i ) {\
            write_char( pad_char );\
        }\
        padding = 0;\
    } while(0)

    while( remaining ) {
        usize brace_index = 0;
        if( !___find_char( remaining, at, '{', &brace_index ) ) {
            write_string( remaining, at );
            break;
        }
        assert( remaining >= brace_index );

        write_string( brace_index, at );

        at        += brace_index;
        remaining -= brace_index;

        if( !remaining ) {
            break;
        }

        while( *at && *at != '}' ) {
            intermediate.len = 0;

            advance();
            FMTIdentifier identifier =
                ___determine_identifier( &remaining, &at );
            if( identifier == FMT_IDENT_LITERAL_PAREN ) {
                write_char( '{' );
                goto while_break;
            }

            struct FMTIdentifierArguments args = {};
            if( !___process_arguments(
                &remaining, &at, identifier, &args
            ) ) {
                goto fmt_end;
            }

            #define ___write_int( prefix, is_signed, size, arg_type ) do {\
                prefix##size value   = 0;\
                prefix##size * values = &value;\
                if( args.count ) {\
                    values = va_arg( va, void* );\
                } else {\
                    value = va_arg( va, arg_type );\
                }\
                FormatInteger format =\
                    ___internal_fmt_format_to_fmt_integer( args.format );\
                char padding_char = args.zero_padding ? '0' : ' ';\
                if( args.count > 1 ) {\
                    write_string_literal( "{ " );\
                }\
                usize loop_count = args.count ? args.count : 1;\
                for( usize i = 0; i < loop_count; ++i ) {\
                    union FMTInteger integer = {};\
                    integer.prefix##size = values[i];\
                    ___internal_fmt_integer(\
                        ___write_intermediate, &intermediate,\
                        integer, format,\
                        is_signed, size, args.width );\
                    usize padding = args.padding;\
                    write_string_padded(\
                        padding_char, intermediate.len, intermediate.buffer );\
                    args.padding = padding;\
                    intermediate.len = 0;\
                    if( i + 1 < loop_count ) {\
                        write_string_literal( ", " );\
                    }\
                }\
                if( args.count > 1 ) {\
                    write_string_literal( " }" );\
                }\
            } while(0)

            #define ___write_int_vec( prefix, is_signed, size, component_count ) do {\
                struct FMT##prefix##size##_##component_count\
                    { prefix##size v[component_count]; };\
                struct FMT##prefix##size##_##component_count value = {};\
                struct FMT##prefix##size##_##component_count* value_ptr = &value;\
                if( args.count ) {\
                    value_ptr = va_arg( va, void* );\
                } else {\
                    value = va_arg(\
                        va, struct FMT##prefix##size##_##component_count );\
                }\
                FormatInteger format =\
                    ___internal_fmt_format_to_fmt_integer( args.format );\
                char padding_char = args.zero_padding ? '0' : ' ';\
                if( args.count > 1 ) {\
                    write_string_literal( "{ " );\
                }\
                usize loop_count = args.count ? args.count : 1;\
                for( usize i = 0; i < loop_count; ++i ) {\
                    write_string_literal( "{ " );\
                    union FMTInteger integer = {};\
                    for( usize j = 0; j < component_count; ++j ) {\
                        integer.prefix##size = (value_ptr + i)->v[j];\
                        ___internal_fmt_integer(\
                            ___write_intermediate, &intermediate, integer,\
                            format, is_signed, size, args.width );\
                        usize padding = args.padding;\
                        write_string_padded(\
                            padding_char, intermediate.len, intermediate.buffer );\
                        args.padding = padding;\
                        intermediate.len = 0;\
                        if( j + 1 < component_count ) {\
                            write_string_literal( ", " );\
                        }\
                    }\
                    write_string_literal( " }" );\
                    if( i + 1 < loop_count ) {\
                        write_string_literal( ", " );\
                    }\
                }\
                if( args.count > 1 ) {\
                    write_string_literal( " }" );\
                }\
            } while(0)

            #define ___write_float_vec( component_count ) do {\
                struct FMTv##component_count { f32 v[component_count]; };\
                struct FMTv##component_count  value = {};\
                struct FMTv##component_count* values = &value;\
                if( args.count ) {\
                    values = va_arg( va, void* );\
                } else {\
                    value = va_arg( va, struct FMTv##component_count );\
                }\
                usize loop_count = args.count ? args.count : 1;\
                char padding_char = args.zero_padding ? '0' : ' ';\
                if( args.count > 1 ) {\
                    write_string_literal( "{ " );\
                }\
                for( usize i = 0; i < loop_count; ++i ) {\
                    struct FMTv##component_count current = values[i];\
                    write_string_literal( "{ " );\
                    for( usize j = 0; j < component_count; ++j ) {\
                        ___internal_fmt_float(\
                            ___write_intermediate,\
                            &intermediate,\
                            current.v[j],\
                            args.precision, args.width );\
                        usize padding = args.padding;\
                        write_string_padded(\
                            padding_char, intermediate.len, intermediate.buffer );\
                        args.padding = padding;\
                        intermediate.len = 0;\
                        if( j + 1 < component_count ) {\
                            write_string_literal( ", " );\
                        }\
                    }\
                    write_string_literal( " }" );\
                    if( i + 1 < loop_count ) {\
                        write_string_literal( ", " );\
                    }\
                }\
                if( args.count > 1 ) {\
                    write_string_literal( " }" );\
                }\
            } while(0)

            switch( identifier ) {
                case FMT_IDENT_BOOL: {
                    b32  local_value = false;
                    b32* value       = &local_value;
                    if( args.count ) {
                        value = va_arg( va, void* );
                    } else {
                        local_value = va_arg( va, int ) != 0;
                    }

                    if( args.count > 1 ) {
                        write_string_literal( "{ " );
                    }

                    usize loop_count = args.count ? args.count : 1;
                    for( usize i = 0; i < loop_count; ++i ) {
                        b32 current = value[i];

                        usize len = args.is_binary ? 1 :
                            ( current ? sizeof("true"): sizeof("false") ) - 1;
                        char* string = args.is_binary ?
                            ( current == true ? "1" : "0" ) :
                            ( current == true ? "true" : "false" );

                        i64 padding = args.padding;
                        write_string_padded( ' ', len, string );

                        if( i + 1 < loop_count ) {
                            write_string_literal( ", " );
                            args.padding = padding;
                        }
                    }

                    if( args.count > 1 ) {
                        write_string_literal( " }" );
                    }
                } break;

                case FMT_IDENT_CHAR: {
                    char  local_value = 0;
                    char* value = &local_value;
                    if( args.count ) {
                        value = va_arg( va, void* );
                    } else {
                        local_value = va_arg( va, int );
                    }

                    if( args.count > 1 ) {
                        write_string_literal( "{ " );
                    }

                    usize loop_count = args.count ? args.count : 1;
                    for( usize i = 0; i < loop_count; ++i ) {
                        char current = value[i];
                        switch( args.casing ) {
                            case FMT_FORMAT_CASE_UPPER: {
                                current = ___char_to_upper( current );
                            } break;
                            case FMT_FORMAT_CASE_LOWER: {
                                current = ___char_to_lower( current );
                            } break;
                            default: break;
                        }
                        i64 padding = args.padding;
                        write_string_padded( ' ', 1, &current );

                        if( i + 1 < loop_count ) {
                            write_string_literal( ", " );
                            args.padding = padding;
                        }
                    }

                    if( args.count > 1 ) {
                        write_string_literal( " }" );
                    }
                } break;

                case FMT_IDENT_STRING_SLICE:
                case FMT_IDENT_CSTR: {
                    usize len   = 0;
                    char* value = NULL;

                    if( identifier == FMT_IDENT_CSTR ) {
                        value = va_arg( va, void* );
                        if( !value ) {
                            value = "";
                        }
                        len = ___strlen( value );
                        if( args.count && args.count < len ) {
                            len = args.count;
                        }
                    } else {
                        struct ___S { char* buffer; usize len, capacity; };
                        struct ___S ss = va_arg( va, struct ___S );
                        len   = ss.len;
                        value = ss.buffer;

                        if( args.count ) {
                            if( ss.capacity ) {
                                if( args.count < ss.capacity ) {
                                    len = args.count;
                                }
                            } else {
                                if( args.count < ss.len ) {
                                    len = args.count;
                                }
                            }
                        }
                    }

                    if( !args.casing ) {
                        write_string_padded( ' ', len, value );
                        break;
                    }

                    if( args.padding < 0 ) {
                        apply_padding( ' ', args.padding, len );
                    }
                    switch( args.casing ) {
                        case FMT_FORMAT_CASE_UPPER: {
                            for( usize i = 0; i < len; ++i ) {
                                write_char( ___char_to_upper( value[i] ) );
                            }
                        } break;
                        case FMT_FORMAT_CASE_LOWER: {
                            for( usize i = 0; i < len; ++i ) {
                                write_char( ___char_to_lower( value[i] ) );
                            }
                        } break;
                        default: break;
                    }
                    if( args.padding > 0 ) {
                        apply_padding( ' ', args.padding, len );
                    }
                } break;

                case FMT_IDENT_INT8: {
                    ___write_int( i, true, 8, i32 );
                } break;
                case FMT_IDENT_INT16: {
                    ___write_int( i, true, 16, i32 );
                } break;
                case FMT_IDENT_INT32:
                case FMT_IDENT_INT: {
                    ___write_int( i, true, 32, i32 );
                } break;
                case FMT_IDENT_INT64: {
                    ___write_int( i, true, 64, i64 );
                } break;

                case FMT_IDENT_UINT8: {
                    ___write_int( u, false, 8, u32 );
                } break;
                case FMT_IDENT_UINT16: {
                    ___write_int( u, false, 16, u32 );
                } break;
                case FMT_IDENT_UINT32:
                case FMT_IDENT_UINT: {
                    ___write_int( u, false, 32, u32 );
                } break;
                case FMT_IDENT_UINT64: {
                    ___write_int( u, false, 64, u64 );
                } break;

                case FMT_IDENT_INT_VECTOR_2: {
                    ___write_int_vec( i, true, 32, 2 );
                } break;
                case FMT_IDENT_INT_VECTOR_3: {
                    ___write_int_vec( i, true, 32, 3 );
                } break;
                case FMT_IDENT_INT_VECTOR_4: {
                    ___write_int_vec( i, true, 32, 4 );
                } break;

                case FMT_IDENT_UINT_VECTOR_2: {
                    ___write_int_vec( u, false, 32, 2 );
                } break;
                case FMT_IDENT_UINT_VECTOR_3: {
                    ___write_int_vec( u, false, 32, 3 );
                } break;
                case FMT_IDENT_UINT_VECTOR_4: {
                    ___write_int_vec( u, false, 32, 4 );
                } break;

                case FMT_IDENT_FLOAT64:
                case FMT_IDENT_FLOAT32:
                case FMT_IDENT_FLOAT: {
                    f64   value  = 0.0;
                    void* values = &value;
                    usize stride = sizeof(f32);
                    if( identifier == FMT_IDENT_FLOAT64 ) {
                        stride = sizeof(f64);
                    }

                    FMTStorage storage = 0;

                    if( args.count ) {
                        values = va_arg( va, void* );
                    } else {
                        value = va_arg( va, f64 );

                        if( args.format == FMT_FORMAT_MEMORY ) {
                            value = ___determine_storage( value, &storage );
                        }
                    }

                    if( args.count > 1 ) {
                        write_string_literal( "{ " );
                    }

                    usize loop_count = args.count ? args.count : 1;
                    char padding_char = args.zero_padding ? '0' : ' ';

                    for( usize i = 0; i < loop_count; ++i ) {
                        void* ptr = ((u8*)values + ( i * stride ));
                        f64 current;
                        if( identifier == FMT_IDENT_FLOAT64 ) {
                            current = *(f64*)ptr;
                        } else {
                            if( args.count ) {
                                current = *(f32*)ptr;
                            } else {
                                current = *(f64*)ptr;
                            }
                        }

                        ___internal_fmt_float(
                            ___write_intermediate,
                            &intermediate, current, args.precision, args.width );
                        usize padding = args.padding;

                        if( args.format == FMT_FORMAT_MEMORY ) {
                            usize unit_len = sizeof( " B" ) - 1;
                            char* unit     = " B";
                            switch( storage ) {
                                case FMT_STORAGE_BYTES: break;
                                case FMT_STORAGE_KB: {
                                    unit_len = sizeof( " KB" ) - 1;
                                    unit     = " KB";
                                } break;
                                case FMT_STORAGE_MB: {
                                    unit_len = sizeof( " MB" ) - 1;
                                    unit     = " MB";
                                } break;
                                case FMT_STORAGE_GB: {
                                    unit_len = sizeof( " GB" ) - 1;
                                    unit     = " GB";
                                } break;
                                case FMT_STORAGE_TB: {
                                    unit_len = sizeof( " TB" ) - 1;
                                    unit     = " TB";
                                } break;
                            }
                            ___write_intermediate( &intermediate, unit_len, unit );
                        }

                        write_string_padded(
                            padding_char, intermediate.len, intermediate.buffer );
                        args.padding = padding;
                        intermediate.len = 0;

                        if( i + 1 < loop_count ) {
                            write_string_literal( ", " );
                        }
                    }

                    if( args.count > 1 ) {
                        write_string_literal( " }" );
                    }
                } break;

                case FMT_IDENT_VECTOR_2: {
                    ___write_float_vec( 2 );
                } break;
                case FMT_IDENT_VECTOR_3: {
                    ___write_float_vec( 3 );
                } break;
                case FMT_IDENT_VECTOR_4: {
                    ___write_float_vec( 4 );
                } break;

                default: panic();
            }

            // if there is no closing brace,
            // format string is invalid and formatting should
            // be aborted.
            check_end();
        }
        // skip over closing brace.
        advance();
    while_break:
        continue;
    }
fmt_end:

    return result;
}
LD_API usize ___internal_fmt_write(
    FormatWriteFN* write, void* target,
    usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );
    usize result =
        ___internal_fmt_write_va( write, target, format_len, format, va );
    va_end( va );
    return result;
}

internal b32 ___read_increment( usize* value, usize max ) {
    usize increment_result = *value + 1;
    if( increment_result >= max ) {
        return false;
    }
    *value = increment_result;
    return true;
}

LD_API b32 fmt_read_int( usize len, char* buffer, i64* out_parsed_int ) {
    if( !len || !buffer ) {
        return false;
    }

    b32 is_negative = false;
    i64 result = 0;

    usize index = 0;
    if( buffer[index] == '-' ) {
        if( !___read_increment( &index, len ) ) {
            return false;
        }
        is_negative = true;
    }

    do {
        if( !___char_is_number( buffer[index] ) ) {
            if( !index ) {
                return false;
            }
            break;

        }
        result *= 10;
        result += buffer[index] - '0';
    } while( ___read_increment( &index, len ) );

    *out_parsed_int = result * ( is_negative ? -1 : 1 );
    return true;
}
LD_API b32 fmt_read_uint( usize len, char* buffer, u64* out_parsed_int ) {
    if( !len || !buffer ) {
        return false;
    }

    u64 result = 0;
    usize index = 0;

    do {
        if( !___char_is_number( buffer[index] ) ) {
            if( !index ) {
                return false;
            }
            break;

        }
        result *= 10;
        result += buffer[index] - '0';
    } while( ___read_increment( &index, len ) );

    *out_parsed_int = result;
    return true;
}

global char FMT_DIGITS_BINARY[2] = { '0', '1' };
global char FMT_DIGITS_DECIMAL[10] = {
    '0', '1', '2',
    '3', '4', '5',
    '6', '7', '8',
    '9'
};
global char FMT_DIGITS_HEXADECIMAL_UPPER[16] = {
    '0', '1', '2',
    '3', '4', '5',
    '6', '7', '8',
    '9', 'A', 'B',
    'C', 'D', 'E',
    'F'
};
global char FMT_DIGITS_HEXADECIMAL_LOWER[16] = {
    '0', '1', '2',
    '3', '4', '5',
    '6', '7', '8',
    '9', 'a', 'b',
    'c', 'd', 'e',
    'f'
};
#define FMT_BASE_BINARY      (2)
#define FMT_BASE_DECIMAL     (10)
#define FMT_BASE_HEXADECIMAL (16)

#define ___fmt_push( character ) do {\
    if( !buffer || (len + 1 >= capacity) ) {\
        result++;\
    } else {\
        buffer[len++] = character;\
    }\
} while(0)

internal maybe_unused
void ___internal_fmt_reverse( usize len, char* buffer ) {
    char* begin = buffer;
    char* end   = begin + len - 1;

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

internal
b32 ___is_nan64( f64 x ) {
    u64 bitpattern = reinterpret_cast( u64, &x );

    u64 exp = bitpattern & F64_EXPONENT_MASK;
    u64 man = bitpattern & F64_MANTISSA_MASK;

    return exp == F64_EXPONENT_MASK && man != 0;
}
internal
usize ___internal_fmt_float(
    FormatWriteFN* write, void* target, f64 f,
    u32 precision, FMTFormatWidth width
) {
    // TODO(alicia): try to implement Grisu3 or some equivalent algorithm
    unused(width);
    #define FMT_FLOAT_MAX_PRECISION (12)
    u32 precision_left = precision;
    if( precision_left > FMT_FLOAT_MAX_PRECISION ) {
        precision_left = FMT_FLOAT_MAX_PRECISION;
    }

    #define FMT_FLOAT_BUFFER_SIZE (128)
    usize index = FMT_FLOAT_BUFFER_SIZE - precision_left;
    char  buffer[FMT_FLOAT_BUFFER_SIZE] = {};

    usize precision_index = index;

    #define ___float_push( c )\
        assert( index );\
        buffer[--index] = c
    #define ___float_push_fractional( c )\
        buffer[precision_index++] = c

    #define ___float_write()\
        write( target, FMT_FLOAT_BUFFER_SIZE - index, buffer + index )

    if( ___is_nan64( f ) ) {
        return write( target, sizeof("NaN") - 1, "NaN" );
    } else if( f == F64_POS_INFINITY ) {
        return write( target, sizeof("INF") - 1, "INF" );
    } else if( f == F64_NEG_INFINITY ) {
        return write( target, sizeof("-INF") - 1, "-INF" );
    }

    f64 f_abs = f;
    if( f < 0 ) {
        f_abs = -f;
    }

    u64   base   = 10;
    char* digits = FMT_DIGITS_DECIMAL;

    u64 whole_part = (u64)f_abs;
    f64 fractional = f_abs - (f64)whole_part;

    if( precision_left ) {
        fractional *= 10.0;
        u64 fractional_digit = (u64)fractional;
        while( precision_left ) {
            usize digit_index = fractional_digit % base;
            ___float_push_fractional( digits[digit_index] );
            fractional *= 10.0;
            fractional_digit = (u64)fractional;
            precision_left--;
        }
        ___float_push( '.' );
    }

    usize whole_parts_written = 0;
    if( !whole_part ) {
        ___float_push( digits[0] );
    } else while( whole_part ) {
        usize digit_index = whole_part % base;
        if( width == FMT_FORMAT_WIDTH_SEPARATOR ) {
            if( whole_parts_written && !(whole_parts_written % 3) ) {
                ___float_push( ',' );
            }
        }
        ___float_push( digits[digit_index] );
        whole_part /= base;

        whole_parts_written++;
    }

    if( f < 0 ) {
        ___float_push( '-' );
    }

    return ___float_write();
}

internal
usize ___internal_fmt_integer(
    FormatWriteFN* write, void* target,
    union FMTInteger integer, FormatInteger format,
    b32 is_signed, u32 size, FMTFormatWidth width
) {
    #define FMT_INTEGER_BUFFER_SIZE (64 + 2 + 10) // 2 is 0x/0b and 10 is separators
    usize index     = FMT_INTEGER_BUFFER_SIZE;
    char buffer[FMT_INTEGER_BUFFER_SIZE] = {};
    #define ___integer_push( c )\
        assert( index );\
        buffer[--index] = c

    #define ___integer_write()\
        write( target, FMT_INTEGER_BUFFER_SIZE - index, buffer + index )
        
    if( !integer.u64 ) {
        ___integer_push( '0' );
        switch( format ) {
            case FORMAT_INTEGER_BINARY: {
                ___integer_push( 'b' );
                ___integer_push( '0' );
            } break;

            case FORMAT_INTEGER_DECIMAL: break;

            case FORMAT_INTEGER_HEXADECIMAL_LOWER:
            case FORMAT_INTEGER_HEXADECIMAL_UPPER: {
                ___integer_push( 'x' );
                ___integer_push( '0' );
            } break;
        }
        return ___integer_write();
    }

    u64   base   = FMT_BASE_DECIMAL;
    char* digits = FMT_DIGITS_DECIMAL;

    char  separator = 0;
    usize separator_frequency = 0;
    switch( format ) {
        case FORMAT_INTEGER_BINARY: {
            base   = FMT_BASE_BINARY;
            digits = FMT_DIGITS_BINARY;
            separator_frequency = 8;
            separator = '\'';
        } break;
        case FORMAT_INTEGER_DECIMAL: {
            separator_frequency = 3;
            separator = ',';
        } break;
        case FORMAT_INTEGER_HEXADECIMAL_LOWER:
        case FORMAT_INTEGER_HEXADECIMAL_UPPER: {
            base   = FMT_BASE_HEXADECIMAL;
            digits = format == FORMAT_INTEGER_HEXADECIMAL_LOWER ?
                FMT_DIGITS_HEXADECIMAL_LOWER :
                FMT_DIGITS_HEXADECIMAL_UPPER;
            separator_frequency = 4;
            separator = '\'';
        } break;
    }

    #define ___fmt_integer( size ) do {\
        u##size value = 0;\
        if( is_signed && integer.i##size < 0 ) {\
            if( format == FORMAT_INTEGER_DECIMAL ) {\
                value = -integer.i##size;\
            } else {\
                value = *(u##size*)&integer.i##size;\
            }\
        } else {\
            value = integer.u##size;\
        }\
        usize write_count = 0;\
        usize max_digit_count = size;\
        if(\
            format == FORMAT_INTEGER_HEXADECIMAL_LOWER ||\
            format == FORMAT_INTEGER_HEXADECIMAL_UPPER\
        ) {\
            max_digit_count /= 4;\
        }\
        loop {\
            u##size digit_index = value % base;\
            if(\
                width == FMT_FORMAT_WIDTH_SEPARATOR &&\
                (!(write_count % separator_frequency) && write_count )\
            ) {\
                ___integer_push( separator );\
            }\
            ___integer_push( digits[digit_index] );\
            write_count++;\
            value /= base;\
            if(\
                width == FMT_FORMAT_WIDTH_NORMAL ||\
                ( width == FMT_FORMAT_WIDTH_SEPARATOR &&\
                format == FORMAT_INTEGER_DECIMAL )\
            ) {\
                if( !value ) {\
                    break;\
                }\
            } else if(\
                width == FMT_FORMAT_WIDTH_FULL ||\
                width == FMT_FORMAT_WIDTH_SEPARATOR\
            ) {\
                if( write_count == max_digit_count ) {\
                    if( buffer[index] == separator ) {\
                        index++;\
                    }\
                    break;\
                }\
            }\
        }\
        if( is_signed && integer.i##size < 0 ) {\
            if( format == FORMAT_INTEGER_DECIMAL ) {\
                ___integer_push( '-' );\
            }\
        }\
    } while(0)

    switch( size ) {
        case 8: {
            ___fmt_integer( 8 );
        } break;
        case 16: {
            ___fmt_integer( 16 );
        } break;
        case 32: {
            ___fmt_integer( 32 );
        } break;
        case 64: {
            ___fmt_integer( 64 );
        } break;
        default: panic();
    }

    switch( format ) {
        case FORMAT_INTEGER_BINARY: {
            ___integer_push( 'b' );
            ___integer_push( '0' );
        } break;
        case FORMAT_INTEGER_DECIMAL: break;
        case FORMAT_INTEGER_HEXADECIMAL_LOWER:
        case FORMAT_INTEGER_HEXADECIMAL_UPPER: {
            ___integer_push( 'x' );
            ___integer_push( '0' );
        } break;
    }
   
    return ___integer_write();
}

LD_API usize fmt_write_i8(
    FormatWriteFN* write, void* target, i8 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .i8 = integer },
        format, true, 8, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_u8(
    FormatWriteFN* write, void* target, u8 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .u8 = integer },
        format, false, 8, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_i16(
    FormatWriteFN* write, void* target, i16 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .i16 = integer },
        format, true, 16, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_u16(
    FormatWriteFN* write, void* target, u16 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .u16 = integer },
        format, false, 16, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_i32(
    FormatWriteFN* write, void* target, i32 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .i32 = integer },
        format, true, 32, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_u32(
    FormatWriteFN* write, void* target, u32 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .u32 = integer },
        format, false, 32, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_i64(
    FormatWriteFN* write, void* target, i64 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .i64 = integer },
        format, true, 64, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_u64(
    FormatWriteFN* write, void* target, u64 integer, FormatInteger format
) {
    return ___internal_fmt_integer(
        write, target, (union FMTInteger){ .u64 = integer },
        format, false, 64, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_float(
    FormatWriteFN* write, void* target, f64 f, u32 precision
) {
    return ___internal_fmt_float(
        write, target, f, precision, FMT_FORMAT_WIDTH_NORMAL );
}
LD_API usize fmt_write_bool(
    FormatWriteFN* write, void* target, b32 b, b32 binary
) {
    usize len = b ?
        ( binary ? 1 : sizeof("true") - 1 ) : ( binary ? 1 : sizeof("false") - 1 );
    char* cstr = b ? ( binary ? "1" : "true" ) : ( binary ? "0" : "false" );
    return write( target, len, cstr );
}

internal b32 ___find_char(
    usize len, const char* cstr, char character, usize* out_index
) {
    b32 found = false;
    usize index = 0;
#if LD_SIMD_WIDTH == 1 || !defined(LD_ARCH_X86)
    for( usize i = 0; i < len; ++i ) {
        if( cstr[i] == character ) {
            index = i;
            found = true;
            break;
        }
    }

    if( found ) {
        *out_index = index;
    }
    return found;
#elif LD_SIMD_WIDTH == 4
    usize size16    = len / 16;
    usize remainder = len % 16;

    __m128i* cstr_ptr   = (__m128i*)cstr;
    __m128i wide_char   = _mm_set1_epi8( character );

    for( usize i = 0; i < size16; ++i ) {
        __m128i loaded_string = _mm_loadu_si128( cstr_ptr );
        __m128i cmp_result    = _mm_cmpeq_epi8( loaded_string, wide_char );

        u16 mask = _mm_movemask_epi8( cmp_result );

        if( mask ) {
            u16 offset = 0;
            // if match was found in second half (8 characters)
            // of format string chunk.
            // this is just so we skip to second half
            // when checking each bit for where the character is
            if( !(mask & (0xFF)) && (mask & (0xFF << 8)) ) {
                index += 8;
                offset = 8;
            }

            // check every bit until first match is found.
            for( u16 j = 0; j < 8; ++j ) {
                if( (mask >> (j + offset)) & 0x1 ) {
                    break;
                }
                index++;
            }

            *out_index = index;
            found      = true;
            break;
        }

        cstr_ptr++;
        index += 16;
    }

    if( found ) {
        *out_index = index;
        return true;
    }

    char* at = (char*)cstr_ptr;
    for( usize i = 0; i < remainder; ++i ) {
        if( at[i] == character ) {
            *out_index = index;
            return true;
        }
        index++;
    }

    return false;
#endif
}


