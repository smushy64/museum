# * Description:  Makefile for Project Museum
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023
# * Notes:        

MAKEFLAGS += -s

# valid arch: x86_64, arm, wasm
export TARGET_ARCH     := x86_64
export IS_DEBUG        := true
export BUILD_PATH      := build/$(if $(IS_DEBUG),debug,release)
export PROJECT_VERSION_MAJOR := 0
export PROJECT_VERSION_MINOR := 1
export PROJECT_VERSION := $(PROJECT_VERSION_MAJOR).$(PROJECT_VERSION_MINOR)
export ENGINE_NAME     := LiquidEngine

export VULKAN_VERSION_MAJOR := 1
export VULKAN_VERSION_MINOR := 2

export GL_VERSION_MAJOR := 4
export GL_VERSION_MINOR := 5

export project_version_underscore := $(subst .,_,$(PROJECT_VERSION))

ifeq ($(OS), Windows_NT)
	export dll_ext := .dll
	export exe_ext := .exe
	export windows := true
	export win_res := $(BUILD_PATH)/obj/resources.o

	export HOST_OS_NAME := Windows
else
	export dll_ext := .so
	export exe_ext :=
endif

export EXECUTABLE_NAME := ProjectMuseum_$(project_version_underscore)_$(if $(IS_DEBUG),DEBUG,)$(exe_ext)

export testbed_name := TestBed_$(project_version_underscore)_$(if $(IS_DEBUG),DEBUG,)$(exe_ext)

# c++ compiler
export CXX  := clang++ -std=c++20
# c compiler
export CC   := clang -std=c11
# pch compiler
export PCHC := $(CL)
# linker
export CL   := $(CXX)

export INCLUDE_PATHS := -I../liquid_engine

RELEASE_C_FLAGS := -O2

DEBUG_C_FLAGS := -O0 -g -gcodeview -Wall -Wextra
DEBUG_C_FLAGS += -Wno-missing-braces -pedantic -Werror
DEBUG_C_FLAGS += -Wno-c11-extensions -Wno-gnu-zero-variadic-macro-arguments
DEBUG_C_FLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-unused-variable

C_FLAGS := -fno-rtti -fno-exceptions -Werror=vla -ffast-math
C_FLAGS += -fno-operator-names -fno-strict-enums
C_FLAGS += -MMD -MP
ifeq ($(TARGET_ARCH), x86_64)
	C_FLAGS += -masm=intel -march=native
endif
ifeq ($(TARGET_ARCH), arm)
	C_FLAGS += -target-arm64
endif
ifeq ($(TARGET_ARCH), wasm)
	C_FLAGS += -target-wasm64
endif

RELEASE_CPP_FLAGS :=
DEBUG_CPP_FLAGS   := -DDEBUG -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING
CPP_FLAGS         := -DLD_SIMD_WIDTH=4 
CPP_FLAGS         += -DLIQUID_ENGINE_VERSION=\""$(ENGINE_NAME) $(PROJECT_VERSION)"\"
CPP_FLAGS         += -DLIQUID_ENGINE_VERSION_MAJOR=$(PROJECT_VERSION_MAJOR)
CPP_FLAGS         += -DLIQUID_ENGINE_VERSION_MINOR=$(PROJECT_VERSION_MINOR)

RELEASE_LINKER_FLAGS :=
DEBUG_LINKER_FLAGS   := -g -fuse-ld=lld -Wl,//debug 
LINKER_FLAGS         := -nostdlib++ -nostdlib -lkernel32
# tell the linker to allocate a megabyte for the stack
LINKER_FLAGS += -Wl,//stack:0x100000

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

export preprocessor_flags := $(if $(IS_DEBUG),$(DEBUG_CPP_FLAGS),$(RELEASE_CPP_FLAGS)) $(CPP_FLAGS)
export release_compiler_flags := $(RELEASE_C_FLAGS)
export debug_compiler_flags   := $(DEBUG_C_FLAGS)
export common_compiler_flags  := $(C_FLAGS)
export compiler_flags     := $(if $(IS_DEBUG),$(DEBUG_C_FLAGS),$(RELEASE_C_FLAGS)) $(C_FLAGS)
export pch_flags          := -x c++-header
export linker_debug_flags := $(if $(IS_DEBUG),$(DEBUG_LINKER_FLAGS),$(RELEASE_LINKER_FLAGS))
export linker_flags       := $(linker_debug_flags) $(LINKER_FLAGS)
export obj_path           := ../$(BUILD_PATH)/obj

export liquid_engine_dll := $(ENGINE_NAME)_$(project_version_underscore)$(if $(IS_DEBUG),_DEBUG,)$(dll_ext)

# @echo "Make: Compiler flags: " $(compiler_flags)
# @echo "Make: Pre-processor flags: " $(preprocessor_flags)
# @echo "Make: Linker flags: " $(linker_flags)
all:
	@echo "Make: Target architecture:" $(HOST_OS_NAME)-$(TARGET_ARCH)
	@$(MAKE) --directory=liquid_engine --no-print-directory
	@$(MAKE) --directory=testbed --no-print-directory
	@$(MAKE) --directory=shader --no-print-directory

# TODO(alicia): run!

run: all
	@echo Make: running Project Museum $(PROJECT_VERSION) . . .
	@echo
	@echo not yet implemented!

test: all
	@echo Make: running Test Bed $(PROJECT_VERSION) . . .
	@echo
	@./$(BUILD_PATH)/$(testbed_name) --gl

debug: all $(if windows,debug_win32,)

debug_win32:
	@echo Make: running Project Museum $(PROJECT_VERSION) in RemedyBG . . .
	@echo
	@remedybg start-debugging 1

clean:
	@echo Make: removing everything from build directory . . .
	@rm -r -f build/debug/*
	@rm -r -f build/release/*
	@rm -r -f resources/shaders/*

# for debugging variables
spit:
	@echo $(LINKER_FLAGS)

help:
	@echo Help for Project Museum Makefile
	@echo "   all:   compile everything"
	@echo "   run:   compile and run \"Project Museum\" executable"
	@echo "   debug: compile and run \"Project Museum\" in debugger"
	@echo "   test:  compile and run \"Test Bed\" executable"
	@echo "   clean: delete everything in build directory"

.PHONY: all run test debug debug_win32 clean help spit

