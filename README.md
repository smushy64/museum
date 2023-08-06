<!--
 * Description:  Readme
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# Project Museum [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Project Museum is a 3D Action RPG. Liquid Engine is the custom engine that will power this game, all written in C-style C++.

Compiled using clang, C++20 standard.

No libraries are used with the exception of system libraries ( Win32, OpenGL etc. )*

`* currently using [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) for font rendering`

## Building

### Requirements
Currently, only Windows is supported as a build target

#### Common requirements
- clang 16.0.1*
- GNU Make 4.4.1*
- [glslc v2023.1*](https://github.com/google/shaderc/blob/main/downloads.md)
#### Windows requirements
- [msys2](https://www.msys2.org/wiki/MSYS2-installation/)
- windres 2.40*
    - should be installed with mingw by default
#### Optional
- [RemedyBG Debugger](https://remedybg.itch.io/remedybg)
- [gf gdb frontend for linux](https://github.com/nakst/gf)

`*: might work with other versions`

### Steps

1) cd into root directory
2) build all sources: ```make```
3) executable will be in ./build/debug/

to build and execute testbed, run: ```make test```

to list other provided make recipes, run: ```make help```

More information on how to build is available [here](./BUILD.md).

## Documentation

Liquid Engine API documentation can be found [here](./docs/toc.md).

## Special Thanks

- Casey Muratori ([Handmade Hero](https://www.youtube.com/@MollyRocket/))
- Travis Vroman ([Kohi Game Engine](https://www.youtube.com/@TravisVroman/))
- George Menhorn ([RemedyBG Debugger](https://remedybg.itch.io/remedybg))

