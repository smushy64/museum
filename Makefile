# * Description:  Makefile for Liquid Platform Layer
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

# 1 MB
export LD_STACK_SIZE := 0x100000

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

export BUILD_PATH := build/$(if $(RELEASE),release,debug)
export OBJ_PATH   := $(BUILD_PATH)/obj

CFLAGS := -Werror -Wall -Wextra -pedantic -Werror=vla
CFLAGS += -fno-strict-enums -Wno-missing-braces
CFLAGS += -Wno-c11-extensions -Wno-gnu-zero-variadic-macro-arguments
CFLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types
CFLAGS += -Wno-ignored-attributes -Wno-gnu-case-range
CFLAGS += -Wno-fixed-enum-extension -Wno-static-in-inline
CFLAGS += -Wno-c99-extensions -Wno-duplicate-decl-specifier
CFLAGS += -Wno-gnu-empty-initializer

ifeq ($(RELEASE), true)
	CFLAGS += -O2 -ffast-math
else
	CFLAGS += -O0 -g
endif

ifeq ($(TARGET_ARCH), x86_64)
	CFLAGS += -masm=intel -mcpu=x86-64
else
	CFLAGS += -mcpu=$(TARGET_ARCH)
endif

ifeq ($(TARGET_PLATFORM), win32)
	ifeq ($(RELEASE), true)
		CFLAGS += -mwindows
	else
		CFLAGS += -gcodeview
	endif
endif

export VK_MAJOR := 1
export VK_MINOR := 2

export GL_MAJOR := 4
export GL_MINOR := 5

export LD_EXE_NAME       := liquid-engine$(if $(RELEASE),,-debug)
export LD_EXE            := $(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export TARGET            := $(BUILD_PATH)/$(LD_EXE)
export LIB_CORE_NAME     := liquid-core$(if $(RELEASE),,-debug)
export LIB_CORE          := $(LIB_CORE_NAME).$(SO_EXT)
export LIB_PACKAGER_NAME := liquid-packager
export LIB_PACKAGER      := $(LIB_PACKAGER_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export LIB_TESTBED_NAME  := testbed$(if $(RELEASE),,-debug)
export LIB_TESTBED       := $(LIB_TESTBED_NAME).$(SO_EXT)

CPPFLAGS := -DLD_SIMD_WIDTH=4
CPPFLAGS += -DLIQUID_ENGINE_VERSION=\""$(LD_NAME) $(LD_VERSION)"\"
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MAJOR=$(LD_MAJOR)
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MINOR=$(LD_MINOR)
CPPFLAGS += -DLIQUID_ENGINE_EXECUTABLE=\"$(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)\"
CPPFLAGS += -DGL_VERSION_MAJOR=$(GL_MAJOR)
CPPFLAGS += -DGL_VERSION_MINOR=$(GL_MINOR)
CPPFLAGS += -DVULKAN_VERSION_MAJOR=$(VK_MAJOR)
CPPFLAGS += -DVULKAN_VERSION_MINOR=$(VK_MINOR)
CPPFLAGS += -DLD_EXPORT
CPPFLAGS += -DLIQUID_ENGINE_CORE_LIBRARY_PATH=\"$(LIB_CORE)\"
CPPFLAGS += -DLD_PLATFORM_INTERNAL
CPPFLAGS += -DSTACK_SIZE=$(LD_STACK_SIZE)

ifeq ($(RELEASE), true)
else
	CPPFLAGS += -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING -DLD_CONSOLE_APP
	CPPFLAGS += -DLD_DEVELOPER_MODE
endif

LDFLAGS := 

ifeq ($(TARGET_PLATFORM), win32)
	LDFLAGS += -fuse-ld=lld -nostdlib -lkernel32
	LDFLAGS += -mstack-probe-size=999999999 -Wl,//stack:$(LD_STACK_SIZE)

	ifeq ($(RELEASE), true)
		LDFLAGS += -Wl,//release
	else
		LDFLAGS += -Wl,//debug
	endif
endif

INCLUDE := -Ishared -Iliquid_platform

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

export SHARED_HEADERS := $(notdir $(call recurse,./shared,*.h) )
LOCAL_SHARED_HEADERS  := $(addprefix ./shared/,$(SHARED_HEADERS))

all: $(if $(DEPENDENCIES_ONLY),generate_dependencies,shaders $(if $(SHADER_ONLY),,$(TARGET) build_core build_package))

generate_dependencies:
	@$(MAKE) --directory=liquid_engine generate_dependencies
	@$(MAKE) --directory=testbed generate_dependencies

test: all
	@$(MAKE) --directory=testbed --no-print-directory
	# @$(if $(shell which remedybg), remedybg start-debugging,)

build_core: 
	@$(MAKE) --directory=liquid_engine --no-print-directory

build_package:
	@$(MAKE) --directory=liquid_package --no-print-directory

shaders:
	@$(MAKE) --directory=shaders --no-print-directory

run:
	@echo run none

spit:
	@echo "platform:     "$(TARGET_PLATFORM)
	@echo "arch:         "$(TARGET_ARCH)
	@echo "build path:   "$(BUILD_PATH)
	@echo "c compiler:   "$(CC)
	@echo "c standard:   "$(CSTD)
	@echo "c++ compiler: "$(CXX)
	@echo "c++ standard: "$(CXXSTD)
	@echo
	@echo "-------- shared --------------"
	@echo
	@echo "headers:    "$(SHARED_HEADERS)
	@echo
	@echo "-------- platform ------------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     "$(CFLAGS)
	@echo
	@echo "cppflags:   "$(CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    "$(LDFLAGS)
	@echo
	@echo "main:       "$(LD_PLATFORM_MAIN)
	@$(MAKE) --directory=liquid_engine spit
	@$(MAKE) --directory=shaders spit
	@$(MAKE) --directory=testbed spit
	@$(MAKE) --directory=liquid_package spit

clean: $(if $(DEPENDENCIES_ONLY),clean_dependencies,$(if $(SHADER_ONLY),,clean_files) clean_shaders) 

clean_dependencies:
	@$(MAKE) --directory=liquid_engine clean_dependencies
	@$(MAKE) --directory=testbed clean_dependencies

clean_shaders:
	@echo "Make: cleaning "$(if $(RELEASE),release,debug)" shaders . . ."
	-@rm -f ./$(BUILD_PATH)/resources/shaders/{*,.*} 2> /dev/null || true

clean_files:
	@echo "Make: cleaning "$(if $(RELEASE),release,debug)" files . . ."
	-@rm -f ./$(BUILD_PATH)/{*,.*} 2> /dev/null || true
	-@rm -f ./$(OBJ_PATH)/{*,.*} 2> /dev/null || true

help:
	@echo "Arguments:"
	@echo "  all:    compile everything"
	@echo "  run:    compile and run project museum"
	@echo "  test:   compile and run testbed"
	@echo "  clean:  clean build directory"
	@echo "  help:   display this message"
	@echo
	@echo "Options:"
	@echo "  RELEASE=true          build/clean only for release mode"
	@echo "  TARGET_ARCH=...       set target architecture"
	@echo "                            valid values: x86_64, arm64, wasm64"
	@echo "                            default: current architecture"
	@echo "  TARGET_PLATFORM=...   set target platform"
	@echo "                            valid values: win32, linux, macos, wasm"
	@echo "                            default: current platform"
	@echo "  SHADER_ONLY=          build/clean only shaders"
	@echo "                            valid values: true"
	@echo "                            default: "
	@echo "  DEPENDENCIES_ONLY=    build/clean only dependencies"
	@echo "                            valid values: true"
	@echo "                            default: "

IS_WINDOWS     :=
WIN32RESOURCES := 

ifeq ($(TARGET_PLATFORM), win32)
	IS_WINDOWS     := true
	WIN32RESOURCES := $(OBJ_PATH)/win32_resources.o
endif

$(WIN32RESOURCES):win32/resources.rc
	@echo "Make: compiling win32 resources . . ."
	@mkdir -p $(OBJ_PATH)
	@windres win32/resources.rc -o $(WIN32RESOURCES)

$(TARGET): $(if $(IS_WINDOWS),$(WIN32RESOURCES),) $(LD_PLATFORM_MAIN) $(LOCAL_SHARED_HEADERS) ./liquid_platform/platform.h
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(LD_PLATFORM_MAIN) $(WIN32RESOURCES) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all test shaders run clean clean_shaders clean_files help build_core clean_dependencies generate_dependencies

