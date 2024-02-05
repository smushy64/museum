#if !defined(LD_CORE_SORT_H)
#define LD_CORE_SORT_H
/**
 * Description:  Sorting algorithms
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 20, 2023
*/
#include "shared/defines.h"

/// Less than function used for sorting.
typedef b32 SortLTFN( void* lhs, void* rhs, void* params );
/// Swap elements function used for sorting.
typedef void SortSwapFN( void* lhs, void* rhs );
/// Quicksort sorting algorithm implementation.
CORE_API void sorting_quicksort(
    isize from_inclusive,
    isize to_inclusive,
    usize element_size,
    void* buffer,
    SortLTFN* lt,
    void* opt_lt_params,
    SortSwapFN* swap
);
/// Quicksort unsigned int32
CORE_API void sorting_quicksort_u32(
    isize from_inclusive, isize to_inclusive, u32* buffer );

/// Reverse items in buffer.
CORE_API void sorting_reverse(
    usize item_count, usize item_size,
    void* buffer, void* temp_buffer );

#endif /* header guard */
