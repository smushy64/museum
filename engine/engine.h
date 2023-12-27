#if !defined(LD_CORE_ENGINE_H)
#define LD_CORE_ENGINE_H
/*
 * Description:  Engine Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
*/
#include "shared/defines.h"

/// Request engine to exit program.
LD_API void engine_exit(void);

/// Set fullscreen mode.
LD_API void engine_set_fullscreen( b32 is_fullscreen );
/// Query current fullscreen mode.
LD_API b32 engine_query_fullscreen(void);
/// Toggle fullscreen mode.
LD_API void engine_toggle_fullscreen(void);

#endif /* header guard */
