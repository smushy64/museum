# Building

## Requirements
Currently, only Windows is supported as a build target

### Common requirements
- clang 16.0.1*
- GNU Make 4.4.1*
- [glslc v2023.1*](https://github.com/google/shaderc/blob/main/downloads.md)
### Windows requirements
- [msys2](https://www.msys2.org/wiki/MSYS2-installation/)
- windres 2.40*
    - should be installed with mingw by default
### Optional
- [RemedyBG Debugger](https://remedybg.itch.io/remedybg)

`*: might work with other versions`

## Building 
### Debug build

1) cd into root directory
2) build all sources: ```make```
3) executable will be in ./build/debug/

to build and execute testbed, run: ```make test```

to list other provided make recipes, run: ```make help```

### Release build

1) cd into root directory
2) build all sources: ```make RELEASE=true```
3) executable will be in ./build/release/

to build and execute testbed, run: ```make test RELEASE=true```

to list other provided make recipes, run: ```make help```

## Debugging
I do not use Visual Studio or any other debugger except
RemedyBG so I don't what the right steps are for those debuggers.

That being said, debug builds provide .pdb files on Windows
and object/executable files should build with debug symbols
appropriate for gdb on *nix platforms.

### RemedyBG
Under Session/Application and Parameters, set 'Command'
to 'build/debug/LiquidEngine_x_x_debug_win32.exe', x_x being the
current version of LiquidEngine.

Press F10 to begin debugging and break on entry point.

Additionally, the command argument
(again, under Session/Application and Parameters)
--output-debug-string is recommended to enable print out to the
debugger's output window.

