<!--*
 * Description:  Todo list for project museum
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
-->
# TODO 

## Features
- [x] Hello Triangle (OpenGL)
  - [x] Shader compilation + linking (pipelines?)
  - [x] Vertex buffer + index buffer rendering
- [x] Custom string formatting functions (snprintf, printf etc.)
- [x] Properly document new formatting functions
- [x] Replace all printf/snprintf with new formatting functions
- [x] format: String left justify + padding
- [x] custom putc stdout/stderr
    - Win32: WriteConsole()
- [x] Remove dependency on stdint.h on x86
- [x] Debug auto scope timer
- [x] Remove dependency on the C runtime library in liquid engine library code
- [x] Macros that hide specfic intrinsics when using SIMD
- [x] Implement mutex
- [ ] Proper frametiming (using platform_sleep?)
- [ ] Audio subsystem
- [ ] Memory arena
    - [x] Stack Arena
    - [ ] Dynamic Arena
- [ ] Materials system
- [ ] Text rendering
- [ ] ECS or something approximating it
- [ ] Resource manager
- [ ] Write logging to file (timestamps?)
- [ ] Customizable input actions + input processing
- [ ] custom variadic macro implementations
- [ ] format: more accurate float print out
- [ ] Remove dependency on the C runtime library in testbed code
- [ ] Implement custom math functions
    - asin, acos, atan, atan2
    - pow, log, mod

## Bug Fixes
- [ ] format: integer binary format should only print necessary bits

## Optimzations
- [x] Poll XInputGetState on a separate thread
    so that the main thread doesn't stall
    when a controller isn't connected. This is a known bug with
    XInputGetState so I gotta work around it.
- [ ] Optimize string formatting functions
- [ ] replace hardware trig functions with taylor series + simd
    - on my machine, the hardware instructions are faster than stdlib
        but that may not be the case on other machines

