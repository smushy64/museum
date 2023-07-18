# Project Museum [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Project Museum is a 3D Action RPG. Liquid Engine is the custom engine that will power this game, all written in C-style C++.

Compiled using clang, C++20 standard.

No libraries are used with the exception of system libraries ( Win32, OpenGL etc. )

## Building

### Requirements
Currently, only Windows is supported as a build target

#### Common requirements
- clang 16.0.1* (mingw version on Windows)
- GNU Make 4.4.1*
- glslc (can be installed with Vulkan SDK or [here](https://github.com/google/shaderc/blob/main/downloads.md))
#### Windows requirements
- mingw64
- windres (should be installed with mingw by default)

`*: might work on other versions`

### Building 
#### Debug build
cd into root directory and,

to build all sources
``` sh
make
```

to build and run testbed 
``` sh
make test
```

to list other provided make recipes
``` sh
make help
```

More information on how to build is available [here](./BUILD.md).

## Special Thanks

- Casey Muratori ([Handmade Hero](https://www.youtube.com/@MollyRocket/))
- Travis Vroman ([Kohi Game Engine](https://www.youtube.com/@TravisVroman/))
- George Menhorn ([RemedyBG Debugger](https://remedybg.itch.io/remedybg))

