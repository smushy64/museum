<!--*
 * Description:  Todo list for project museum
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
-->
# TODO 

## Motivation mini-projects
- [ ] Make an asteroids clone using liquid engine :)
    - [x] i-frames on respawn
    - [x] lives counter
    - [x] font rendering
        - [x] stb_truetype packed font atlas
        - [x] score
        - [x] main menu
    - [x] make lives counter a ui element
    - [x] prevent window resize
    - [ ] sfx
    - [ ] ufo?

## Linux Build
- [ ] Link testbed shared object to executable properly
- [ ] Rewrite with platform.h changes

## Library to EXE + Unity build
- [x] Make Liquid Engine the executable and Testbed the library
- [x] Change build system to unity build instead of incremental build
- [x] Compile windows resources into executable
- [x] parse --load= option in engine.cpp

## Bug Fixes
- [ ] format: integer binary format should only print necessary bits
- [ ] replace all __builtin_ functions throughout math library
    - clangd doesn't complain about them but during compilation,
    any function that uses them will cause a linker error
- [ ] make sure that atan actually produces good results.
    outside the -1 -> 1 range, it diverges heavily.
    it may not actually be that big of a problem but still.
- [x] normalize_range does not seem to do what i think it does
    - it does do what i think 
- [x] defines: why is size_t defined as usize?
    - not really sure why that was there but i removed it and nothing broke (i think?)

## Optimzations
- [ ] Optimize string formatting functions
- [x] Poll XInputGetState on a separate thread
    so that the main thread doesn't stall
    when a controller isn't connected. This is a known bug with
    XInputGetState so I gotta work around it.
- [x] replace hardware trig functions with taylor series + simd
    - on my machine, the hardware instructions are faster than stdlib
        but that may not be the case on other machines

## Features
- [x] --help argument
- [x] put all platform functions into platform.h
- [x] expose threading functions to game dll
    - [x] semaphore api
    - [x] mutex api
    - [x] interlocked api
- [x] Hello Triangle (OpenGL)
  - [x] Shader compilation + linking (pipelines?)
  - [x] Vertex buffer + index buffer rendering
- [x] Custom string formatting functions (snprintf, printf etc.)
- [x] Properly document new formatting functions
- [x] Replace all printf/snprintf with new formatting functions
- [x] format: String left justify + padding
- [x] custom putc stdout/stderr
- [x] Remove dependency on stdint.h on x86
- [x] Debug auto scope timer
- [x] Remove dependency on the C runtime library in liquid engine library code
- [x] Macros that hide specfic intrinsics when using SIMD
- [x] Implement mutex
- [x] Implement math functions: asin, acos, atan, atan2, powi, modf
- [x] Stack Arena
- [ ] ECS or something approximating it
- [ ] Proper frametiming (using platform_sleep?)
- [ ] Audio subsystem
- [ ] Materials system
- [ ] Text rendering
- [ ] Resource manager
- [ ] Write logging to file (timestamps?)
- [ ] Customizable input actions + input processing
- [ ] custom variadic macro implementations
- [ ] format: more accurate float print out
- [ ] Completely rewrite memory heap allocations,
not liking how data about allocations is stored
- [ ] Implement custom math functions
    - log, powf, no simd sqrtf

