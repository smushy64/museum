/**
 * Description:  Sorting algorithm implementations
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 20, 2023
*/
#include "shared/defines.h"
#include "core/sort.h"

internal isize sorting_quicksort_partition(
    isize low, isize high, usize element_size, void* buffer,
    SortLTFN* lt, void* opt_lt_params, SortSwapFN* swap_
) {
    u8* buf = buffer;
    void* pivot = buf + (high * element_size);

    isize i = (isize)low - 1;

    for( isize j = (isize)low; j <= (isize)high - 1; ++j ) {
        void* at_j = buf + (j * element_size);

        if( lt( at_j, pivot, opt_lt_params ) ) {
            ++i;
            void* at_i = buf + (i * element_size);
            swap_( at_i, at_j );
        }
    }

    swap_( buf + ((i + 1) * element_size), buf + (high * element_size) );
    return i + 1;
}

CORE_API void sorting_quicksort(
    isize       low,
    isize       high,
    usize       element_size,
    void*       buffer,
    SortLTFN*   lt,
    void*       opt_lt_params,
    SortSwapFN* swap_
) {
    while( low < high ) {
        isize partition_index = sorting_quicksort_partition(
            low, high, element_size,
            buffer, lt, opt_lt_params, swap_ );
        if( partition_index - low < high - partition_index ) {
            sorting_quicksort(
                low, partition_index - 1, element_size,
                buffer, lt, opt_lt_params, swap_ );
            low = partition_index + 1;
        } else {
            sorting_quicksort(
                partition_index + 1, high, element_size,
                buffer, lt, opt_lt_params, swap_ );
            high = partition_index - 1;
        }
    }
}

internal isize sorting_quicksort_partition_u32(
    isize low, isize high, u32* buffer
) {

    u32* pivot = buffer + high;

    isize i = low - 1;

    for( isize j = low; j <= high - 1; ++j ) {
        u32* at_j = buffer + j;

        if( *at_j < *pivot ) {
            ++i;
            u32* at_i = buffer + i;
            u32 temp = *at_j;
            *at_j = *at_i;
            *at_i = temp;
        }
    }

    u32 temp = *(buffer + ( i + 1 ));
    *(buffer + (i + 1)) = *(buffer + high);
    *(buffer + high) = temp;

    return i + 1;
}

CORE_API void sorting_quicksort_u32( isize low, isize high, u32* buffer ) {
    while( low < high ) {
        isize partition_index = sorting_quicksort_partition_u32( low, high, buffer );
        if( partition_index - low < high - partition_index ) {
            sorting_quicksort_u32( low, partition_index - 1, buffer );
            low = partition_index + 1;
        } else {
            sorting_quicksort_u32( partition_index + 1, high, buffer );
            high = partition_index - 1;
        }
    }
}

