#if !defined(LD_CORE_SORT_H)
#define LD_CORE_SORT_H
/**
 * Description:  Sorting algorithms
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 20, 2023
*/
#include "defines.h"

/// Less than function used for sorting.
typedef b32 SortLTFN( void* lhs, void* rhs, void* params );
/// Swap elements function used for sorting.
typedef void SortSwapFN( void* lhs, void* rhs );
/// Quicksort sorting algorithm implementation.
LD_API void sorting_quicksort(
    isize from_inclusive,
    isize to_inclusive,
    usize element_size,
    void* buffer,
    SortLTFN* lt,
    void* opt_lt_params,
    SortSwapFN* swap
);
/// Quicksort unsigned int32
LD_API void sorting_quicksort_u32(
    isize from_inclusive, isize to_inclusive, u32* buffer );

#endif /* header guard */
