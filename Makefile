# * Description:  Makefile for Project Museum
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023
# * Notes:        

MAKEFLAGS += -s

export IS_DEBUG        := true
export BUILD_PATH      := build/$(if $(IS_DEBUG),debug,release)
export PROJECT_VERSION_MAJOR := 0
export PROJECT_VERSION_MINOR := 1
export PROJECT_VERSION := $(PROJECT_VERSION_MAJOR).$(PROJECT_VERSION_MINOR)
export ENGINE_NAME     := LiquidEngine

export project_version_underscore := $(subst .,_,$(PROJECT_VERSION))
ifeq ($(OS), Windows_NT)
	export dll_ext := .dll
	export exe_ext := .exe
	export windows := true
	export win_res := $(BUILD_PATH)/obj/resources.o
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
DEBUG_C_FLAGS   := -O0 -g -gcodeview -Wall -Wextra -Wno-missing-braces
C_FLAGS         := -march=native -MMD -MP

RELEASE_CPP_FLAGS :=
DEBUG_CPP_FLAGS   := -DDEBUG -DLD_LOGGING -DSM_ASSERTIONS -DLD_OUTPUT_DEBUG_STRING -DLD_PROFILING
CPP_FLAGS         := -DSM_SIMD_WIDTH=4 -DUNICODE
CPP_FLAGS += -DLIQUID_ENGINE_VERSION=\""$(ENGINE_NAME) $(PROJECT_VERSION)"\"
CPP_FLAGS += -DLIQUID_ENGINE_VERSION_MAJOR=$(PROJECT_VERSION_MAJOR)
CPP_FLAGS += -DLIQUID_ENGINE_VERSION_MINOR=$(PROJECT_VERSION_MINOR)

RELEASE_LINKER_FLAGS :=
DEBUG_LINKER_FLAGS   := -g -fuse-ld=lld -Wl,//debug
LINKER_FLAGS         := -static-libstdc++ -static-libgcc -lmingw32

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

export preprocessor_flags := $(if $(IS_DEBUG),$(DEBUG_CPP_FLAGS),$(RELEASE_CPP_FLAGS)) $(CPP_FLAGS)
export compiler_flags     := $(if $(IS_DEBUG),$(DEBUG_C_FLAGS),$(RELEASE_C_FLAGS)) $(C_FLAGS)
export pch_flags          := -x c++-header
export linker_flags       := $(if $(IS_DEBUG),$(DEBUG_LINKER_FLAGS),$(RELEASE_LINKER_FLAGS)) $(LINKER_FLAGS)
export obj_path           := ../$(BUILD_PATH)/obj

export liquid_engine_dll := $(ENGINE_NAME)_$(project_version_underscore)$(if $(IS_DEBUG),_DEBUG,)$(dll_ext)

all:
	@$(MAKE) --directory=liquid_engine --no-print-directory
	@$(MAKE) --directory=testbed --no-print-directory

# TODO(alicia): run!

run: all
	@echo Make: running Project Museum $(PROJECT_VERSION) . . .
	@echo
	@echo not yet implemented!

test: all
	@echo Make: running Test Bed $(PROJECT_VERSION) . . .
	@echo
	@./$(BUILD_PATH)/$(testbed_name)

debug: all $(if windows,debug_win32,)

debug_win32:
	@echo Make: running Project Museum $(PROJECT_VERSION) in RemedyBG . . .
	@echo
	@remedybg start-debugging 1

clean:
	@echo Make: removing everything from build directory . . .
	@rm -r -f build/debug/*
	@rm -r -f build/release/*

# for debugging variables
spit:
	@echo $(subst \,/,$(VULKAN_SDK))

help:
	@echo Help for Project Museum Makefile
	@echo "   all:   compile everything"
	@echo "   run:   compile and run \"Project Museum\" executable"
	@echo "   debug: compile and run \"Project Museum\" in debugger"
	@echo "   test:  compile and run \"Test Bed\" executable"
	@echo "   clean: delete everything in build directory"

.PHONY: all run test debug debug_win32 clean help spit

