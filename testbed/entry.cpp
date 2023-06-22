// * Description:  Testbed Entry Point Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 21, 2023
#include "entry.h"
#include <core/collections.h>

b32 entry(
    struct EngineContext*   engine_ctx,
    struct ThreadWorkQueue* thread_work_queue,
    struct RenderOrder*     render_order,
    struct Time* time,
    void*  user_params
) {
    unused(engine_ctx);
    unused(thread_work_queue);
    unused(render_order);
    unused(time);
    unused(user_params);
    return true;
}

