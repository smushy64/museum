#if !defined(TESTBED_ENTRY_HPP)
#define TESTBED_ENTRY_HPP
// * Description:  Testbed Entry Point
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 21, 2023
#include "pch.h"

b32 entry(
    struct EngineContext*   engine_ctx,
    struct ThreadWorkQueue* thread_work_queue,
    struct RenderOrder*     render_order,
    struct Time* time,
    void*  user_params
);

#endif // header guard
