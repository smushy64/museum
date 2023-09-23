# * Description:  Liquid Engine Makefile
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 21, 2023

MAKEFLAGS += -j -s

export CC     := clang
export CSTD   := -std=c99
export CXX    := clang++
export CXXSTD := -std=c++20

export RELEASE ?=

export LD_MAJOR        := 0
export LD_MINOR        := 2
export LD_VERSION      := $(LD_MAJOR).$(LD_MINOR)
export LD_NAME         := liquid-engine
export LD_VERSION_PATH := $(subst .,-,$(LD_VERSION))

export LD_MEMORY_PAGE_SIZE := 4096

# x86_64, arm64, wasm64
ifndef $(TARGET_ARCH)
	ifeq ($(OS), Windows_NT)
		# NOTE(alicia): can't find a good uname alternative
		# that is builtin on Windows :(
		export TARGET_ARCH := x86_64
	else
		export TARGET_ARCH := $(shell uname -m)
	endif
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

	LDMAIN := liquid_platform/platform_win32.c
else
	export EXE_EXT :=
	export SO_EXT  := so

	ifeq ($(TARGET_PLATFORM), linux)
		LDMAIN := liquid_platform/platform_linux.c
		LD_MEMORY_PAGE_SIZE := $(shell getconf PAGESIZE)
	endif
endif

export BUILD_PATH := build/$(if $(RELEASE),release,debug)
export OBJ_PATH   := $(BUILD_PATH)/obj

# NOTE(alicia): idk about these C_FLAGs
# -Wno-incompatible-library-redeclaration
CFLAGS := -Werror -Wall -Wextra -pedantic -Werror=vla
CFLAGS += -fno-strict-enums -Wno-missing-braces
CFLAGS += -Wno-c11-extensions -Wno-gnu-zero-variadic-macro-arguments
CFLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types
CFLAGS += -Wno-ignored-attributes -Wno-gnu-case-range
CFLAGS += -Wno-fixed-enum-extension -Wno-static-in-inline
CFLAGS += -Wno-c99-extensions -Wno-duplicate-decl-specifier
CFLAGS += -Wno-gnu-empty-initializer
CFLAGS += -MMD -MP -MF ./$(OBJ_PATH)/$(LD_NAME).d

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

export LD_EXE_NAME := liquid-engine$(if $(RELEASE),,-debug)
export TARGET      := $(BUILD_PATH)/$(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)
export LIB_CORE_NAME := liquid-core$(if $(RELEASE),,-debug)
export LIB_CORE      := $(LIB_CORE_NAME).$(SO_EXT)

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
CPPFLAGS += -DPLATFORM_MEMORY_PAGE_SIZE=$(LD_MEMORY_PAGE_SIZE)
CPPFLAGS += -DLIQUID_ENGINE_CORE_LIBRARY_PATH=\"$(LIB_CORE)\"

ifeq ($(RELEASE), true)
else
	CPPFLAGS += -DDEBUG -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING
endif

ifeq ($(TARGET_PLATFORM), linux)
	CPPFLAGS += -D_XOPEN_SOURCE=600

endif

export LIB_TESTBED_NAME := testbed-$(LD_VERSION_PATH)-$(TARGET_PLATFORM)-$(TARGET_ARCH)$(if $(RELEASE),,-debug)
export LIB_TESTBED      := $(LIB_TESTBED_NAME).$(SO_EXT)

LDFLAGS := 

ifeq ($(TARGET_PLATFORM), win32)
	LDFLAGS += -fuse-ld=lld -nostdlib -lkernel32 -mstack-probe-size=999999999 -Wl,//stack:0x100000

	ifeq ($(RELEASE), true)
		LDFLAGS += -Wl,//release
	else
		LDFLAGS += -Wl,//debug
	endif

endif

INCLUDE := -Iliquid_engine -Iliquid_platform

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

dependencies := $(call recurse,$(OBJ_PATH),*.d)

all: shaders $(if $(SHADER_ONLY),,$(TARGET))

# test: build_testbed
# 	@mkdir -p $(BUILD_PATH)/resources/shaders
# 	@cp resources/shaders/ldcolor.vert.spv $(BUILD_PATH)/resources/shaders/ldcolor.vert.spv
# 	@cp resources/shaders/ldcolor.frag.spv $(BUILD_PATH)/resources/shaders/ldcolor.frag.spv
# 	@cp resources/shaders/post_process.vert.spv $(BUILD_PATH)/resources/shaders/post_process.vert.spv
# 	@cp resources/shaders/post_process.frag.spv $(BUILD_PATH)/resources/shaders/post_process.frag.spv
# 	@cp resources/shaders/phong.vert.spv $(BUILD_PATH)/resources/shaders/phong.vert.spv
# 	@cp resources/shaders/phong.frag.spv $(BUILD_PATH)/resources/shaders/phong.frag.spv
# 	@cp resources/shaders/shadow_directional.vert.spv $(BUILD_PATH)/resources/shaders/shadow_directional.vert.spv
# 	@cp resources/shaders/shadow_directional.frag.spv $(BUILD_PATH)/resources/shaders/shadow_directional.frag.spv
# 	@cp resources/shaders/shadow_point.vert.spv $(BUILD_PATH)/resources/shaders/shadow_point.vert.spv
# 	@cp resources/shaders/shadow_point.geom.spv $(BUILD_PATH)/resources/shaders/shadow_point.geom.spv
# 	@cp resources/shaders/shadow_point.frag.spv $(BUILD_PATH)/resources/shaders/shadow_point.frag.spv
# ifeq ($(TARGET_PLATFORM), win32)
# 	@remedybg start-debugging
# endif

build_testbed: all
	@$(MAKE) --directory=testbed --no-print-directory

shaders:
	@$(MAKE) --directory=shaders --no-print-directory

run:
	@echo run none

spit:
	@echo "platform:   "$(TARGET_PLATFORM)
	@echo "arch:       "$(TARGET_ARCH)
	@echo "page size:  "$(MEMORY_PAGE_SIZE)
	@echo "build path: "$(BUILD_PATH)
	@echo "target:     "$(TARGET)
	@echo "compiler:   "$(CC)
	@echo "standard:   "$(CSTD)
	@echo "cflags:     "$(CFLAGS)
	@echo
	@echo "cppflags:   "$(CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    "$(LDFLAGS)
	@echo
	@echo "main:       "$(LDMAIN)
	# @$(MAKE) --directory=testbed spit
	# @$(MAKE) --directory=shaders spit

clean: $(if $(SHADER_ONLY),clean_shaders, $(if $(RELEASE), clean_shaders clean_release, clean_debug clean_shaders))

clean_shaders:
	@echo "Make: cleaning shaders . . ."
	rm -r -f resources/shaders

clean_debug:
	@echo "Make: cleaning debug directory . . ."
	rm -f $(COREC_PATH)
	rm -r -f build/debug/*

clean_release:
	@echo "Make: cleaning release directory . . ."
	rm -f $(COREC_PATH)
	rm -r -f build/release/*

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
	@echo "  SHADER_ONLY=true      build/clean only shaders"
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

$(TARGET): $(COREC_PATH) $(if $(IS_WINDOWS),$(WIN32RESOURCES),)
	@echo "Make: compiling "$(LD_EXE_NAME).$(EXE_EXT)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(LDMAIN) $(WIN32RESOURCES) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all test shaders run clean clean_shaders clean_debug clean_release help build_testbed

-include $(dependencies)

