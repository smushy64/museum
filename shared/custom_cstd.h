#if !defined(LD_SHARED_CUSTOM_CSTD_H)
#define LD_SHARED_CUSTOM_CSTD_H
/**
 * Description:  Custom C-standard library functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 20, 2023
*/
#include "defines.h"

c_linkage usize strlen( const char* str );
c_linkage void* memcpy( void* restricted dst, const void* restricted src, usize size );
c_linkage void* memmove( void* str1, const void* str2, usize n );
c_linkage void* memset( void* ptr, int value, usize num );
c_linkage char* strcpy( char* restricted dst, const char* restricted src );

#endif /* header guard */
