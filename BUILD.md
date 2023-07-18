# Building

## Requirements
Currently, only Windows is supported as a build target

### Common requirements
- clang 16.0.1* (mingw version on Windows)
- GNU Make 4.4.1*
- glslc (can be installed with Vulkan SDK or [here](https://github.com/google/shaderc/blob/main/downloads.md))
### Windows requirements
- mingw64
- windres (should be installed with mingw by default)

`*: might work on other versions`

## Building 
### Debug build
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

### Release build
edit root Makefile, change
``` make
IS_DEBUG := true
```
to
``` make
IS_DEBUG :=
```
and follow debug build steps

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

