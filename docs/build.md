<!--
 * Description:  Build instructions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
-->

# Build Instructions | [Table of Contents](./readme.md)

## Jump Table
- [Requirements](#requirements)
    - [Common](#common-requirements)
    - [Windows](#windows-requirements)
    - [Optional Software](#optional-software)
- [Debug Build Steps](#debug-build-steps)
- [Release Build Steps](#release-build-steps)
- [Debugging](#debugging)
    - [Entry Points](#entry-points)
    - [RemedyBG](#remedybg)
    - [gf](#gf)
    - [Visual Studio](#visual-studio)

## Requirements
Currently, only windows is supported as a build target.

The linux platform layer has been written in past commits but
I've decided it's better to just focus on one platform layer for the time being.

### Common requirements
- clang 16.0.5 [^1]
- GNU Make 4.4.1 [^1]
- [glslc v2023.1](https://github.com/google/shaderc/blob/main/downloads.md) [^1]
### Windows requirements
- [mingw](https://www.mingw-w64.org/)
- windres 2.41 [^1]
    - should be installed with mingw by default
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

to build and testbed, run:
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
and object/executable files built with debug symbols
appropriate for gdb on other platforms.

### Entry Points
Windows: ../liquid_platform/platform_win32.c:380

Linux: Unavailable

MacOS: Unavailable

### RemedyBG
1) Under Session/Application and Parameters, set 'Command' to
```
./build/debug/liquid-engine-debug.exe
```
2) Under Sesson/Application and Parameters, set 'Command Arguments' to
```
--libload=testbed-debug.dll
```
3) Under Session/Application and Parameters set 'Working Directory' to
```
./build/debug/
```

Additionally, the command argument
--output-debug-string is recommended to enable print out to the
debugger's output window.

Press F10 to begin debugging and break on entry point.

RemedyBG is available [here](https://remedybg.itch.io/remedybg).

### gf
1) Change directory to ./build/debug and run gf2.
2) Set 'Path to Executable' to
```
./liquid-engine-debug
```
3) Set 'Command line arguments' to
```
--libload=testbed-debug.so
```

gf is available [here](https://github.com/nakst/gf).

### Visual Studio
1) Build a debug build and launch it in Visual Studio with
```console
devenv ./build/debug/liquid-engine-debug.exe
```
2) Set working environment to ./build/debug/

Then just run the debugger in the editor.
Sources should automatically load as you step through the debugger.

