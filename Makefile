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

export BUILD_PATH := build/$(if $(RELEASE),release,debug)
export OBJ_PATH   := $(BUILD_PATH)/obj

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

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

# NOTE(alicia): COMMON INFO

export VK_MAJOR := 1
export VK_MINOR := 2

export GL_MAJOR := 4
export GL_MINOR := 5

export LD_EXE_NAME      := liquid$(if $(RELEASE),,-debug)
export LD_EXE           := $(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export TARGET           := $(BUILD_PATH)/$(LD_EXE)
export LIB_CORE_NAME    := liquid-core$(if $(RELEASE),,-debug)
export LIB_CORE         := $(LIB_CORE_NAME).$(SO_EXT)
export LIB_ENGINE_NAME  := liquid-engine$(if $(RELEASE),,-debug)
export LIB_ENGINE       := $(LIB_ENGINE_NAME).$(SO_EXT)
export LIB_TESTBED_NAME := testbed$(if $(RELEASE),,-debug)
export LIB_TESTBED      := $(LIB_TESTBED_NAME).$(SO_EXT)
export UTIL_PKG_NAME    := lpkg
export UTIL_PKG         := $(UTIL_PKG_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export UTIL_HASH_NAME   := lhash
export UTIL_HASH        := $(UTIL_HASH_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)

export GENERATED_DEP_PATH := generated_dependencies.inl

export DLLMAIN := platform/platform_dllmain.c

# NOTE(alicia): COMMON PREPROCESSOR FLAGS

export ENGINE_FLAGS :=	-DLIQUID_ENGINE_VERSION_MAJOR=$(LD_MAJOR) \
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

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DLD_EXPORT -DDLLMAIN_DISABLED
LOCAL_CPPFLAGS += -DLD_PLATFORM_INTERNAL -DLD_CONSOLE_APP

CPPFLAGS := $(ENGINE_FLAGS) $(GRAPHICS_FLAGS) $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

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

LDFLAGS := $(LINKER_FLAGS)

# NOTE(alicia): COMMON INCLUDE FLAGS

export INCLUDE_FLAGS := -I$(shell pwd)

INCLUDE := $(INCLUDE_FLAGS)

# NOTE(alicia): COMMON DEPENDENCIES

export SHARED_H := $(call recurse,./shared,*.h)
export SHARED_C := $(call recurse,./shared,*.c)

export DEP_SHARED_H := $(addprefix .,$(SHARED_H))
export DEP_SHARED_C := $(addprefix .,$(SHARED_C))

export DEP_CORE_H := $(addprefix .,$(call recurse,./core,*.h))
export DEP_CORE_C := $(addprefix .,$(call recurse,./core,*.c))

export PLATFORM_DEP_C := $(addprefix .,$(filter-out ./platform/platform_dllmain.c,$(call recurse,./platform,*.c)))

H := $(call recurse,./platform,*.h)
C := $(call recurse,./platform,*.c)

all: $(TARGET) build_core build_engine build_package build_hash

generate_dependencies:
	@$(MAKE) --directory=liquid_engine generate_dependencies
	@$(MAKE) --directory=testbed generate_dependencies

test: all
	@$(MAKE) --directory=testbed --no-print-directory

build_core:
	@$(MAKE) --directory=core --no-print-directory

build_hash: build_core
	@$(MAKE) --directory=hash --no-print-directory

build_engine: build_core build_shaders
	@$(MAKE) --directory=engine --no-print-directory

build_package: build_core
	@$(MAKE) --directory=package --no-print-directory

build_shaders:
	@$(MAKE) --directory=shaders --no-print-directory

run:
	@echo run none

splat:
	@echo $(DEP_CORE_H)

spit:
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
	@echo
	@echo "-------- platform ------------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   engine, graphics, developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    linker"
	@echo
	@echo "headers:    "$(H)
	@echo 
	@echo "sources:    "$(C)
	@$(MAKE) --directory=core spit
	@$(MAKE) --directory=engine spit
	@$(MAKE) --directory=testbed spit
	@$(MAKE) --directory=package spit
	@$(MAKE) --directory=hash spit

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

$(TARGET): $(if $(IS_WINDOWS),$(WIN32RESOURCES),) $(H) $(C) $(SHARED_H) $(SHARED_C)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(C) $(WIN32RESOURCES) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)


compile_flags: generate_compile_flags
	@$(MAKE) --directory=core generate_compile_flags
	@$(MAKE) --directory=engine generate_compile_flags
	@$(MAKE) --directory=testbed generate_compile_flags
	@$(MAKE) --directory=package generate_compile_flags
	@$(MAKE) --directory=hash generate_compile_flags

COMPILE_COMMANDS_PATH := ./platform/compile_flags.txt

generate_compile_flags:
	@echo "Make: generating platform "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(CFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all test run clean help \
	build_core build_hash build_package \
	build_engine build_shaders \
	clean_files clean_shaders clean_dependencies \
	generate_dependencies \
	spit splat compile_flags generate_compile_flags

