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
- [ ] Platform agnostic SIMD wrapper?
- [ ] custom variadic macro implementations

## Optimzations
- [ ] Optimize string formatting functions
- [ ] replace hardware trig functions with taylor series + simd
    - on my machine, the hardware instructions are faster than stdlib
        but that may not be the case on other machines

