<!--
 * Description:  Readme
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->
# Project Museum [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Project Museum is a 3D Action RPG. Liquid Engine is the custom engine that powers this game, all written in C and C++.

Liquid Engine is compiled using clang, C99 standard.

Testbed and game code are compiled with clang++, C++20 standard.

No libraries are used with the exception of system libraries ( Win32, OpenGL etc. )*

*: currently using [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) for font rendering

## Project Goals
- Cross platform game engine (Windows, Linux, Mac, WebAssembly)
- 3D Action RPG (think Dark Souls + Zelda)

## Build

### Requirements
Currently, only Windows is supported as a build target

#### Common requirements
- clang 16.0.5*
- GNU Make 4.4.1*
- [glslc v2023.1*](https://github.com/google/shaderc/blob/main/downloads.md)
#### Windows requirements
- [mingw](https://www.mingw-w64.org/)
- windres 2.41*
    - should be installed with mingw by default
#### Optional
- [RemedyBG Debugger](https://remedybg.itch.io/remedybg)
- [gf gdb frontend for linux](https://github.com/nakst/gf)
- [Visual Studio](https://visualstudio.microsoft.com/)

*: might work with other versions

### Steps

1) cd into root directory
2) build all sources:
```console
make all
```
3) executable will be in ./build/debug/

to build and execute testbed, run:
```console
make test
```

to list other provided make recipes, run:
```console
make help
```

More information on how to build is available [here](./BUILD.md).

## Documentation

Liquid Engine API documentation can be found [here](./docs/toc.md).

## Special Thanks

- Casey Muratori ([Handmade Hero](https://www.youtube.com/@MollyRocket/))
- Travis Vroman ([Kohi Game Engine](https://www.youtube.com/@TravisVroman/))
- George Menhorn ([RemedyBG Debugger](https://remedybg.itch.io/remedybg))

