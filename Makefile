# * Description:  Makefile for Liquid Engine and Project Museum
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 21, 2023

MAKEFLAGS += -j -s

export CC     := clang
export CSTD   := -std=c99
export CXX    := clang++
export CXXSTD := -std=c++20

export RELEASE ?=

export LD_MAJOR   := 0
export LD_MINOR   := 2
export LD_VERSION := $(LD_MAJOR).$(LD_MINOR)
export LD_NAME    := liquid-engine

export EXE_NAME   := liquid$(if $(RELEASE),,-debug)

export GENERATED_PATH := ../generated

# 1 MB
export PROGRAM_STACK_SIZE := 0x100000

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

# valid options = x86_64, arm64, wasm64
ifndef $(TARGET_ARCH)
	export TARGET_ARCH := $(shell uname -m)
endif

# win32, linux, wasm
ifndef $(TARGET_PLATFORM)
	ifeq ($(OS), Windows_NT)
		export TARGET_PLATFORM := win32
	else
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S), Linux)
			export TARGET_PLATFORM := linux
		else
			export TARGET_PLATFORM := macos
		endif
	endif
endif

ifeq ($(TARGET_PLATFORM), win32)
	export EXE_EXT := exe
	export SO_EXT  := dll

	export LD_PLATFORM_MAIN := liquid_platform/platform_win32.c
endif

LOCAL_BUILD_PATH := build/$(if $(RELEASE),release,debug)
LOCAL_OBJ_PATH   := $(LOCAL_BUILD_PATH)/obj

export BUILD_PATH := ../$(LOCAL_BUILD_PATH)
export OBJ_PATH   := ../$(LOCAL_OBJ_PATH)

# NOTE(alicia): COMMON COMPILER FLAGS

export WARNING_FLAGS := -Werror -Wall -Wextra -pedantic -Werror=vla\
	-fno-strict-enums -Wno-missing-braces -Wno-c11-extensions\
	-Wno-gnu-zero-variadic-macro-arguments -Wno-gnu-anonymous-struct\
	-Wno-nested-anon-types -Wno-ignored-attributes -Wno-gnu-case-range\
	-Wno-fixed-enum-extension -Wno-static-in-inline -Wno-c99-extensions\
	-Wno-duplicate-decl-specifier -Wno-gnu-empty-initializer\
	-Wno-c2x-extensions

export OPTIMIZATION_FLAGS_RELEASE := -O2 -ffast-math
export OPTIMIZATION_FLAGS_DEBUG   := -O0 -g

ifeq ($(RELEASE), true)
	export OPTIMIZATION_FLAGS := $(OPTIMIZATION_FLAGS_RELEASE)
else
	export OPTIMIZATION_FLAGS := $(OPTIMIZATION_FLAGS_DEBUG)
endif

ifeq ($(TARGET_ARCH), x86_64)
	export ARCH_FLAGS += -masm=intel -msse4.2
else
	export ARCH_FLAGS += -mcpu=$(TARGET_ARCH)
endif

export PLATFORM_FLAGS_WIN32_RELEASE := -mwindows
export PLATFORM_FLAGS_WIN32_DEBUG   := -gcodeview

ifeq ($(RELEASE), true)
	export PLATFORM_FLAGS_WIN32 := $(PLATFORM_FLAGS_WIN32_RELEASE)
else
	export PLATFORM_FLAGS_WIN32 := $(PLATFORM_FLAGS_WIN32_DEBUG)
endif

ifeq ($(TARGET_PLATFORM), win32)
	export PLATFORM_FLAGS := $(PLATFORM_FLAGS_WIN32)
endif

# NOTE(alicia): COMMON INFO

export VK_MAJOR := 1
export VK_MINOR := 2

export GL_MAJOR := 4
export GL_MINOR := 5

export EXE              := $(EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export LIB_CORE_NAME    := liquid-core$(if $(RELEASE),,-debug)
export LIB_CORE         := $(LIB_CORE_NAME).$(SO_EXT)
export LIB_ENGINE_NAME  := liquid-engine$(if $(RELEASE),,-debug)
export LIB_ENGINE       := $(LIB_ENGINE_NAME).$(SO_EXT)
export LIB_MEDIA_NAME   := liquid-media$(if $(RELEASE),,-debug)
export LIB_MEDIA        := $(LIB_MEDIA_NAME).$(SO_EXT)
export LIB_TESTBED_NAME := testbed$(if $(RELEASE),,-debug)
export LIB_TESTBED      := $(LIB_TESTBED_NAME).$(SO_EXT)
export UTIL_PKG_NAME    := lpkg
export UTIL_PKG         := $(UTIL_PKG_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export UTIL_HASH_NAME   := lhash
export UTIL_HASH        := $(UTIL_HASH_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)

export GENERATED_DEP_PATH    := generated_dependencies.inl
export COMPILE_COMMANDS_PATH := compile_flags.txt
export DLLMAIN               := ../platform/platform_dllmain.c

# NOTE(alicia): COMMON PREPROCESSOR FLAGS

export ENGINE_FLAGS := -DLIQUID_ENGINE_VERSION_MAJOR=$(LD_MAJOR) \
	-DLIQUID_ENGINE_VERSION_MINOR=$(LD_MINOR) \
	-DLIQUID_ENGINE_PATH=\"$(LIB_ENGINE)\"\
	-DLIQUID_ENGINE_VERSION=\""$(LD_NAME) $(LD_VERSION)"\"\
	-DSTACK_SIZE=$(PROGRAM_STACK_SIZE)

export GRAPHICS_FLAGS := -DVULKAN_VERSION_MAJOR=$(VK_MAJOR) \
	-DVULKAN_VERSION_MINOR=$(VK_MINOR) \
	-DGL_VERSION_MAJOR=$(GL_MAJOR) \
	-DGL_VERSION_MINOR=$(GL_MINOR)

export DEVELOPER_FLAGS_RELEASE :=
export DEVELOPER_FLAGS_DEBUG   := -DLD_LOGGING -DLD_ASSERTIONS \
	-DLD_PROFILING -DLD_DEVELOPER_MODE

ifeq ($(RELEASE), true)
	export DEVELOPER_FLAGS := $(DEVELOPER_FLAGS_RELEASE)
else
	export DEVELOPER_FLAGS := $(DEVELOPER_FLAGS_DEBUG)
endif

# NOTE(alicia): COMMON LINKER FLAGS

export LINKER_FLAGS_WIN32_RELEASE := -Wl,//release
export LINKER_FLAGS_WIN32_DEBUG   := -Wl,//debug

ifeq ($(RELEASE), true)
	export LINKER_FLAGS_WIN32 := $(LINKER_FLAGS_WIN32_RELEASE)
else
	export LINKER_FLAGS_WIN32 := $(LINKER_FLAGS_WIN32_DEBUG)
endif

export LINKER_FLAGS_PRELUDE_WIN32 := -fuse-ld=lld -nostdlib -lkernel32\
	-mstack-probe-size=999999999 -Wl,//stack:$(PROGRAM_STACK_SIZE)

ifeq ($(TARGET_PLATFORM), win32)
	export LINKER_FLAGS_PRELUDE := $(LINKER_FLAGS_PRELUDE_WIN32)
endif

ifeq ($(TARGET_PLATFORM), win32)
	export LINKER_FLAGS_PLATFORM := $(LINKER_FLAGS_WIN32)
endif

export LINKER_FLAGS := $(LINKER_FLAGS_PRELUDE) $(LINKER_FLAGS_PLATFORM)

# NOTE(alicia): COMMON INCLUDE FLAGS

export INCLUDE_FLAGS := -I$(shell pwd)

# NOTE(alicia): COMMON DEPENDENCIES

export SHARED_H := $(call recurse,./shared,*.h)
export SHARED_C := $(call recurse,./shared,*.c)

export DEP_SHARED_H := $(addprefix .,$(SHARED_H))
export DEP_SHARED_C := $(addprefix .,$(SHARED_C))

export DEP_CORE_H := $(addprefix .,$(call recurse,./core,*.h))
export DEP_CORE_C := $(addprefix .,$(call recurse,./core,*.c))

export PLATFORM_DEP_H := $(addprefix .,$(call recurse,./platform,*.h))
export PLATFORM_DEP_C := $(addprefix .,$(filter-out ./platform/platform_dllmain.c,$(call recurse,./platform,*.c)))

all: build_platform build_core build_engine build_shaders build_package build_hash

build_platform:
	@$(MAKE) --directory=platform --no-print-directory

build_core:
	@$(MAKE) --directory=core --no-print-directory

build_media:
	@$(MAKE) --directory=media --no-print-directory

build_engine: build_core build_hash build_media
	@$(MAKE) --directory=engine --no-print-directory

build_hash: build_core
	@$(MAKE) --directory=hash --no-print-directory

build_package: build_core build_hash
	@$(MAKE) --directory=package --no-print-directory

build_shaders:
	@$(MAKE) --directory=shaders --no-print-directory

test: all
	@$(MAKE) --directory=testbed --no-print-directory

config:
	@echo "platform:     "$(TARGET_PLATFORM)
	@echo "arch:         "$(TARGET_ARCH)
	@echo "build path:   "$(BUILD_PATH)
	@echo "c compiler:   "$(CC)
	@echo "c standard:   "$(CSTD)
	@echo "c++ compiler: "$(CXX)
	@echo "c++ standard: "$(CXXSTD)
	@echo
	@echo "-------- flags --------------"
	@echo
	@echo "warning:      "$(WARNING_FLAGS)
	@echo 
	@echo "optimization: "$(OPTIMIZATION_FLAGS)
	@echo
	@echo "arch:         "$(ARCH_FLAGS)
	@echo
	@echo "platform:     "$(PLATFORM_FLAGS)
	@echo
	@echo "engine:       "$(ENGINE_FLAGS)
	@echo
	@echo "graphics:     "$(GRAPHICS_FLAGS)
	@echo
	@echo "developer:    "$(DEVELOPER_FLAGS)
	@echo
	@echo "linker_prelude: "$(LINKER_FLAGS_PRELUDE)
	@echo
	@echo "linker_platform: "$(LINKER_FLAGS_PLATFORM)
	@echo
	@echo "linker:       linker_prelude, linker_platform"
	@echo
	@echo "include:      "$(INCLUDE_FLAGS)
	@$(MAKE) --directory=platform config
	@$(MAKE) --directory=core config
	@$(MAKE) --directory=media config
	@$(MAKE) --directory=engine config
	@$(MAKE) --directory=testbed config
	@$(MAKE) --directory=package config
	@$(MAKE) --directory=hash config

clean: clean_shaders clean_objects
	@$(MAKE) --directory=package clean

clean_dep:
	@$(MAKE) --directory=core clean_dep
	@$(MAKE) --directory=media clean_dep
	@$(MAKE) --directory=engine clean_dep
	@$(MAKE) --directory=testbed clean_dep
	@$(MAKE) --directory=package clean_dep
	@$(MAKE) --directory=hash clean_dep

clean_shaders:
	@echo "Make: cleaning "$(if $(RELEASE),release,debug)" shaders . . ."
	-@rm -f ./$(LOCAL_BUILD_PATH)/resources/shaders/{*,.*} 2> /dev/null || true

clean_objects:
	@echo "Make: cleaning "$(if $(RELEASE),release,debug)" objects . . ."
	-@rm -f ./$(LOCAL_BUILD_PATH)/{*,.*} 2> /dev/null || true
	-@rm -f ./$(LOCAL_OBJ_PATH)/{*,.*} 2> /dev/null || true

help:
	@echo "Arguments:"
	@echo "  all:      compile executable, core, engine, shaders and utilities"
	@echo "  test:     compile testbed"
	@echo "  clean:    clean build directory"
	@echo "  config:   print configuration for all targets"
	@echo "  init:     generate compile_flags.txt for all targets, only useful for development"
	@echo "  help:     display this message"
	@echo "  help_ex:  show more make recipes"
	@echo "  help_opt: show options for recipes"

help_ex:
	@echo "Extended Help:"
	@echo "  build_shaders: build only shaders"
	@echo "  clean_shaders: clean only shaders"
	@echo "  clean_objects: clean compilation objects (.o, .dll, .lib, .so, .exe, .pdb)"
	@echo "  clean_dep:     clean generated dependencies"

help_opt:
	@echo "Options:"
	@echo "  RELEASE=true          build/clean only for release mode"
	@echo "  TARGET_ARCH=...       set target architecture"
	@echo "                            valid values: x86_64, arm64, wasm64"
	@echo "                            default: current architecture"
	@echo "  TARGET_PLATFORM=...   set target platform"
	@echo "                            valid values: win32, linux, macos, wasm"
	@echo "                            default: current platform"

init:
	@$(MAKE) --directory=platform generate_compile_flags
	@$(MAKE) --directory=core generate_compile_flags
	@$(MAKE) --directory=media generate_compile_flags
	@$(MAKE) --directory=engine generate_compile_flags
	@$(MAKE) --directory=testbed generate_compile_flags
	@$(MAKE) --directory=package generate_compile_flags
	@$(MAKE) --directory=hash generate_compile_flags

.PHONY: all test clean help \
	build_core build_hash build_package \
	build_engine build_shaders build_media \
	clean_objects clean_shaders clean_dep \
	config init \
	help_ex help_opt

