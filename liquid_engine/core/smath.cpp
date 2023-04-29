/**
 * Description:  C++ math library for game development
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: March 16, 2023
*/
#include "smath.h"

#if defined(SM_ARCH_X86) && SM_SIMD_WIDTH != 1
    #include <immintrin.h>
#endif

#if SM_SIMD_WIDTH == 1

quat operator*( quat lhs, quat rhs ) {
    return {
        ( lhs[0] * rhs[0] ) -
            ( lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3] ),
        ( lhs[0] * rhs[1] ) +
            ( lhs[1] * rhs[0] ) +
            ( ( lhs[2] * rhs[3] ) -
            ( lhs[3] * rhs[2] ) ),
        ( lhs[0] * rhs[2] ) +
            ( lhs[2] * rhs[0] ) +
            ( ( lhs[3] * rhs[1] ) -
            ( lhs[1] * rhs[3] ) ),
        ( lhs[0] * rhs[3] ) +
            ( lhs[3] * rhs[0] ) +
            ( ( lhs[1] * rhs[2] ) -
            ( lhs[2] * rhs[1] ) ),
    };
}

mat4 operator+( const mat4& lhs, const mat4& rhs ) {
    mat4 result;
    result.col0 = lhs.col0 + rhs.col0;
    result.col1 = lhs.col1 + rhs.col1;
    result.col2 = lhs.col2 + rhs.col2;
    result.col3 = lhs.col3 + rhs.col3;
    return result;
}
mat4 operator-( const mat4& lhs, const mat4& rhs ) {
    mat4 result;
    result.col0 = lhs.col0 - rhs.col0;
    result.col1 = lhs.col1 - rhs.col1;
    result.col2 = lhs.col2 - rhs.col2;
    result.col3 = lhs.col3 - rhs.col3;
    return result;
}
mat4 operator*( const mat4& lhs, f32 rhs ) {
    mat4 result;
    result.col0 = lhs.col0 * rhs;
    result.col1 = lhs.col1 * rhs;
    result.col2 = lhs.col2 * rhs;
    result.col3 = lhs.col3 * rhs;
    return result;
}
mat4 operator/( const mat4& lhs, f32 rhs ) {
    mat4 result;
    result.col0 = lhs.col0 / rhs;
    result.col1 = lhs.col1 / rhs;
    result.col2 = lhs.col2 / rhs;
    result.col3 = lhs.col3 / rhs;
    return result;
}
mat4 operator*( const mat4& lhs, const mat4& rhs ) {
    return {
        // column - 0
        ( lhs.c[0] * rhs.c[0] )      +
            ( lhs.c[4]  * rhs.c[1] ) +
            ( lhs.c[8]  * rhs.c[2] ) +
            ( lhs.c[12] * rhs.c[3] ),
        ( lhs.c[1] * rhs.c[0] )      +
            ( lhs.c[5]  * rhs.c[1] ) +
            ( lhs.c[9]  * rhs.c[2] ) +
            ( lhs.c[13] * rhs.c[3] ),
        ( lhs.c[2] * rhs.c[0] )      +
            ( lhs.c[6]  * rhs.c[1] ) +
            ( lhs.c[10] * rhs.c[2] ) +
            ( lhs.c[14] * rhs.c[3] ),
        ( lhs.c[3] * rhs.c[0] )      +
            ( lhs.c[7]  * rhs.c[1] ) +
            ( lhs.c[11] * rhs.c[2] ) +
            ( lhs.c[15] * rhs.c[3] ),
        // column - 1
        ( lhs.c[0] * rhs.c[4] )      +
            ( lhs.c[4]  * rhs.c[5] ) +
            ( lhs.c[8]  * rhs.c[6] ) +
            ( lhs.c[12] * rhs.c[7] ),
        ( lhs.c[1] * rhs.c[4] )      +
            ( lhs.c[5]  * rhs.c[5] ) +
            ( lhs.c[9]  * rhs.c[6] ) +
            ( lhs.c[13] * rhs.c[7] ),
        ( lhs.c[2] * rhs.c[4] )      +
            ( lhs.c[6]  * rhs.c[5] ) +
            ( lhs.c[10] * rhs.c[6] ) +
            ( lhs.c[14] * rhs.c[7] ),
        ( lhs.c[3] * rhs.c[4] )      +
            ( lhs.c[7]  * rhs.c[5] ) +
            ( lhs.c[11] * rhs.c[6] ) +
            ( lhs.c[15] * rhs.c[7] ),
        // column - 2
        ( lhs.c[0] * rhs.c[8] )       +
            ( lhs.c[4]  * rhs.c[9] )  +
            ( lhs.c[8]  * rhs.c[10] ) +
            ( lhs.c[12] * rhs.c[11] ),
        ( lhs.c[1] * rhs.c[8] )       +
            ( lhs.c[5]  * rhs.c[9] )  +
            ( lhs.c[9]  * rhs.c[10] ) +
            ( lhs.c[13] * rhs.c[11] ),
        ( lhs.c[2] * rhs.c[8] )       +
            ( lhs.c[6]  * rhs.c[9] )  +
            ( lhs.c[10] * rhs.c[10] ) +
            ( lhs.c[14] * rhs.c[11] ),
        ( lhs.c[3] * rhs.c[8] )       +
            ( lhs.c[7]  * rhs.c[9] )  +
            ( lhs.c[11] * rhs.c[10] ) +
            ( lhs.c[15] * rhs.c[11] ),
        // column - 3
        ( lhs.c[0] * rhs.c[12] )      +
            ( lhs.c[4]  * rhs.c[13] ) +
            ( lhs.c[8]  * rhs.c[14] ) +
            ( lhs.c[12] * rhs.c[15] ),
        ( lhs.c[1] * rhs.c[12] )      +
            ( lhs.c[5]  * rhs.c[13] ) +
            ( lhs.c[9]  * rhs.c[14] ) +
            ( lhs.c[13] * rhs.c[15] ),
        ( lhs.c[2] * rhs.c[12] )      +
            ( lhs.c[6]  * rhs.c[13] ) +
            ( lhs.c[10] * rhs.c[14] ) +
            ( lhs.c[14] * rhs.c[15] ),
        ( lhs.c[3] * rhs.c[12] )      +
            ( lhs.c[7]  * rhs.c[13] ) +
            ( lhs.c[11] * rhs.c[14] ) +
            ( lhs.c[15] * rhs.c[15] ),
    };
}

#endif // scalar

#if SM_SIMD_WIDTH >= 4

#if defined( SM_ARCH_X86 )

static const __m128 negate_first128 = _mm_setr_ps(
    -1.0f,
    1.0f,
    1.0f,
    1.0f
);

quat operator*( quat lhs, quat rhs ) {
    __m128 a = _mm_setr_ps( lhs.x, lhs.x, lhs.y, lhs.z );
    __m128 b = _mm_setr_ps( rhs.x, rhs.w, rhs.w, rhs.w );
    a = _mm_mul_ps( a, b );

    b = _mm_setr_ps( lhs.y, lhs.y, lhs.z, lhs.x );
    __m128 c = _mm_setr_ps( rhs.y, rhs.z, rhs.x, rhs.y );
    b = _mm_mul_ps( b, c );
    
    c = _mm_setr_ps( lhs.z, lhs.z, lhs.x, lhs.y );
    __m128 d = _mm_setr_ps( rhs.z, rhs.y, rhs.z, rhs.x );
    c = _mm_mul_ps( c, d );

    c = _mm_mul_ps( negate_first128, c );

    d = _mm_sub_ps( b, c );
    a = _mm_add_ps( a, d );

    a = _mm_mul_ps( negate_first128, a );

    __m128 w = _mm_set1_ps( lhs.w );
    b = _mm_setr_ps( rhs.w, rhs.x, rhs.y, rhs.z );
    w = _mm_mul_ps( w, b );

    quat result;
    _mm_storeu_ps( value_pointer(result), _mm_add_ps( w, a ) );
    return result;
}

mat4 operator+( const mat4& lhs, const mat4& rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs0, rhs1, rhs2, rhs3;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs0 = _mm_loadu_ps( value_pointer( rhs.col0 ) );
    rhs1 = _mm_loadu_ps( value_pointer( rhs.col1 ) );
    rhs2 = _mm_loadu_ps( value_pointer( rhs.col2 ) );
    rhs3 = _mm_loadu_ps( value_pointer( rhs.col3 ) );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_add_ps( lhs0, rhs0 ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_add_ps( lhs1, rhs1 ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_add_ps( lhs2, rhs2 ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_add_ps( lhs3, rhs3 ) );

    return result;
}
mat4 operator-( const mat4& lhs, const mat4& rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs0, rhs1, rhs2, rhs3;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs0 = _mm_loadu_ps( value_pointer( rhs.col0 ) );
    rhs1 = _mm_loadu_ps( value_pointer( rhs.col1 ) );
    rhs2 = _mm_loadu_ps( value_pointer( rhs.col2 ) );
    rhs3 = _mm_loadu_ps( value_pointer( rhs.col3 ) );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_sub_ps( lhs0, rhs0 ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_sub_ps( lhs1, rhs1 ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_sub_ps( lhs2, rhs2 ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_sub_ps( lhs3, rhs3 ) );

    return result;
}
mat4 operator*( const mat4& lhs, f32 rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs_;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs_ = _mm_set1_ps( rhs );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_mul_ps( lhs0, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_mul_ps( lhs1, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_mul_ps( lhs2, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_mul_ps( lhs3, rhs_ ) );

    return result;
}
mat4 operator/( const mat4& lhs, f32 rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs_;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs_ = _mm_set1_ps( rhs );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_div_ps( lhs0, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_div_ps( lhs1, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_div_ps( lhs2, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_div_ps( lhs3, rhs_ ) );

    return result;
}
mat4 operator*( const mat4& lhs, const mat4& rhs ) {
    mat4 result = {};

    __m128 a, b, c;

    u32 col = 0;
    u32 row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    col = 1;
    row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    col = 2;
    row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    col = 3;
    row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4_COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4_COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4_COLUMN_COUNT], _mm_add_ps( b, c ) );

    return result;
}

#endif // x86 SSE 

#endif // 4-wide SIMD
