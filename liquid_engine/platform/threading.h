#if !defined(THREADING_HPP)
#define THREADING_HPP
/**
 * Description:  Multi-threading functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

/// read-write fence
SM_API void mem_fence();
/// read fence
SM_API void read_fence();
/// write fence
SM_API void write_fence();

#endif