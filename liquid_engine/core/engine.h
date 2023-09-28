#if !defined(LD_CORE_ENGINE_H)
#define LD_CORE_ENGINE_H
/*
 * Description:  Engine Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
*/
#include "defines.h"

/// Request engine to exit program.
LD_API void engine_exit(void);
/// Query time scale.
LD_API f32 engine_query_time_scale(void);
/// Set time scale.
LD_API void engine_set_time_scale( f32 scale );

#endif /* header guard */
