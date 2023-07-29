<!--
 * Description:  Build instructions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# Building | [Table of Contents](./docs/toc.md)

## Jump Table
- [Requirements](#requirements)
    - [Common Requirements](#common-requirements)
    - [Windows Requirements](#windows-requirements)
    - [Linux Requirements](#linux-requirements)
    - [Optional Software](#optional-software)
- [Debug Build Steps](#debug-build-steps)
- [Release Build Steps](#release-build-steps)
- [Debugging](#debugging)
    - [RemedyBG](#remedybg)
    - [gf](#gf)

## Requirements
Currently, only Windows is supported as a build target

### Common requirements
- clang 16.0.1 [^1]
- GNU Make 4.4.1 [^1]
- [glslc v2023.1](https://github.com/google/shaderc/blob/main/downloads.md) [^1]
### Windows requirements
- [msys2](https://www.msys2.org/wiki/MSYS2-installation/)
- windres 2.40 [^1]
    - should be installed with mingw by default
### Linux requirements
- libX11
- libXcb
- libX11-Xcb
### Optional Software
- [RemedyBG Debugger](https://remedybg.itch.io/remedybg)
- [gf gdb frontend for linux](https://github.com/nakst/gf)

[^1]: might work with other versions

## Debug Build Steps

1) cd into root directory
2) build all sources: ```make```
3) executable will be in ./build/debug/

to build and execute testbed, run: ```make test```

to list other provided make recipes, run: ```make help```

## Release Build Steps

1) cd into root directory
2) build all sources: ```make RELEASE=true```
3) executable will be in ./build/release/

to build and execute testbed, run: ```make test RELEASE=true```

to list other provided make recipes, run: ```make help```

## Debugging
I use RemedyBG on Windows so I don't know what
the right steps are for Visual Studio.

That being said, debug builds provide .pdb files on Windows
and object/executable files build with debug symbols
appropriate for gdb on *nix platforms.

### RemedyBG
Under Session/Application and Parameters, set 'Command'
to 'build/debug/liquid-engine-x-x-debug-win32.exe' where x-x is the
current version of LiquidEngine.

Press F10 to begin debugging and break on entry point.

Additionally, the command argument
(again, under Session/Application and Parameters)
--output-debug-string is recommended to enable print out to the
debugger's output window.

RemedyBG is available [here](https://remedybg.itch.io/remedybg).

### gf
Set Command/Executable to 
'build/debug/liquid-engine-x-x-debug-linux' where x-x is the
current version of LiquidEngine.

gf is available [here](https://github.com/nakst/gf).


