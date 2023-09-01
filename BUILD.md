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
    - [Entry Points](#entry-points)
    - [RemedyBG](#remedybg)
    - [gf](#gf)
    - [Visual Studio](#visual-studio)

## Requirements
Currently, only Windows is supported as a build target

### Common requirements
- clang 16.0.5 [^1]
- GNU Make 4.4.1 [^1]
- [glslc v2023.1](https://github.com/google/shaderc/blob/main/downloads.md) [^1]
### Windows requirements
- [mingw](https://www.mingw-w64.org/)
- windres 2.41 [^1]
    - should be installed with mingw by default
### Linux requirements
- libX11
- libXcb
- libX11-Xcb
### Optional Software
- [RemedyBG Debugger](https://remedybg.itch.io/remedybg)
- [gf gdb frontend for linux](https://github.com/nakst/gf)
- [Visual Studio](https://visualstudio.microsoft.com/)

[^1]: might work with other versions

## Debug Build Steps

1) cd into project root directory
2) build all sources:
``` console
make all
```
3) executable will be in ./build/debug/

to build and execute testbed, run:
``` console
make test
```

to list other provided make recipes, run:
``` console
make help
```

## Release Build Steps

1) cd into project root directory
2) build all sources:
```console
make RELEASE=true
```
3) executable will be in ./build/release/

to build and execute testbed, run:
```console
make test RELEASE=true
```

to list other provided make recipes, run:
```console
make help
```

## Debugging
Debug builds provide .pdb files on Windows
and object/executable files build with debug symbols
appropriate for gdb on *nix platforms.

### Entry Points
Windows: ldwin32main.c at line 288

Linux: Unavailable

Mac: Unavailable

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

### Visual Studio
Build a debug build and launch it in Visual Studio with
```console
devenv build/debug/liquid-engine-x-x-debug-win32.exe
```
where x-x is the current version of Liquid Engine, assuming that you're in the root directory of the project.
Then just run the debugger in the editor.
Sources should automatically load as you step through the debugger.
