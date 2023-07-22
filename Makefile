# * Description:  Makefile for Project Museum
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023

# silent make output
MAKEFLAGS += -s
MAKEFLAGS += -j

export CC := clang++ -std=c++20

RELEASE ?= 
export IS_DEBUG := $(if $(RELEASE),,true)

# valid arch: x86_64, arm, wasm
export TARGET_ARCH := x86_64
export BUILD_PATH  := build/$(if $(IS_DEBUG),debug,release)
export RESOURCES_LOCAL_PATH := resources
export SHADERS_LOCAL_PATH   := $(RESOURCES_LOCAL_PATH)/shaders
export RESOURCES_PATH := $(BUILD_PATH)/$(RESOURCES_LOCAL_PATH)
export SHADERS_PATH   := $(BUILD_PATH)/$(SHADERS_LOCAL_PATH)

export LIQUID_VERSION_MAJOR := 0
export LIQUID_VERSION_MINOR := 1
export LIQUID_VERSION       := $(LIQUID_VERSION_MAJOR).$(LIQUID_VERSION_MINOR)
export LIQUID_NAME          := LiquidEngine
export LIQUID_VERSION_PATH  := $(subst .,_,$(LIQUID_VERSION))

export VULKAN_VERSION_MAJOR := 1
export VULKAN_VERSION_MINOR := 2

export GL_VERSION_MAJOR := 4
export GL_VERSION_MINOR := 5

ifeq ($(OS), Windows_NT)
	export SO_EXT       := .dll
	export EXE_EXT      := .exe
	export IS_WINDOWS   := true
	export HOST_OS_NAME := win32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		export HOST_OS_NAME := linux
		export IS_LINUX := true
	else
		export HOST_OS_NAME := unknown
	endif
	export SO_EXT  := .so
	export EXE_EXT := 
endif

export EXE_NAME := $(LIQUID_NAME)_$(LIQUID_VERSION_PATH)_$(if $(IS_DEBUG),debug_,_)$(HOST_OS_NAME)
export EXE_PATH := $(BUILD_PATH)/$(EXE_NAME)$(EXE_EXT)

export MUSEUM_NAME  := museum_$(if $(IS_DEBUG),debug,release)$(SO_EXT)
export TESTBED_NAME := testbed_$(if $(IS_DEBUG),debug,release)$(SO_EXT)

RC_FLAGS := -O2

DC_FLAGS := -O0 -g -Werror -Wall -Wextra -pedantic
DC_FLAGS += -Wno-missing-braces -Wno-c11-extensions
DC_FLAGS += -Wno-gnu-zero-variadic-macro-arguments
DC_FLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types
DC_FLAGS += -Wno-unused-variable -Wno-ignored-attributes
DC_FLAGS += -Wno-gnu-case-range

ifeq ($(IS_WINDOWS), true)
	DC_FLAGS += -gcodeview
endif

C_FLAGS := -fno-rtti -fno-exceptions -Werror=vla -ffast-math
C_FLAGS += -fno-operator-names -fno-strict-enums
C_FLAGS += -MMD -MP

ifeq ($(IS_LINUX), true)
	C_FLAGS += -fdeclspec
endif

ifeq ($(TARGET_ARCH), x86_64)
	C_FLAGS += -masm=intel -march=native
endif
ifeq ($(TARGET_ARCH), arm)
	C_FLAGS += -target-arm64
endif
ifeq ($(TARGET_ARCH), wasm)
	C_FLAGS += -target-wasm64
endif

RCPP_FLAGS := 

DCPP_FLAGS := -DDEBUG -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING

CPP_FLAGS := -DLD_SIMD_WIDTH=4
CPP_FLAGS += -DLIQUID_ENGINE_VERSION=\""$(LIQUID_NAME) $(LIQUID_VERSION)"\"
CPP_FLAGS += -DLIQUID_ENGINE_VERSION_MAJOR=$(LIQUID_VERSION_MAJOR)
CPP_FLAGS += -DLIQUID_ENGINE_VERSION_MINOR=$(LIQUID_VERSION_MINOR)
CPP_FLAGS += -DGL_VERSION_MAJOR=$(GL_VERSION_MAJOR)
CPP_FLAGS += -DGL_VERSION_MINOR=$(GL_VERSION_MINOR)
CPP_FLAGS += -DVULKAN_VERSION_MAJOR=$(VULKAN_VERSION_MAJOR)
CPP_FLAGS += -DVULKAN_VERSION_MINOR=$(VULKAN_VERSION_MINOR)

RLINK_FLAGS :=
DLINK_FLAGS :=
# TODO(alicia): Temporarily allow stdlib in linux builds until
# it is removed.
LINK_FLAGS  := -nostdlib++

ifeq ($(IS_WINDOWS), true)
	DLINK_FLAGS += -fuse-ld=lld -Wl,//debug
	LINK_FLAGS  += -nostdlib
	LINK_FLAGS  += -fuse-ld=lld -lkernel32 -mstack-probe-size=999999999 -Wl,//stack:0x100000
endif

ifeq ($(IS_LINUX), true)
	LINK_FLAGS += -lX11 -lxcb -lX11-xcb
endif

export c_flags := $(if $(IS_DEBUG),$(DC_FLAGS),$(RC_FLAGS)) $(C_FLAGS)
export rc_flags := $(RC_FLAGS)
export dc_flags := $(DC_FLAGS)
export common_c_flags := $(c_flags)

export link_flags := $(if $(IS_DEBUG),$(DLINK_FLAGS),$(RLINK_FLAGS)) $(LINK_FLAGS)
export rlink_flags := $(RLINK_FLAGS)
export dlink_flags := $(DLINK_FLAGS)
export common_link_flags := $(LINK_FLAGS)

export cpp_flags := $(if $(IS_DEBUG),$(DCPP_FLAGS),$(RCPP_FLAGS)) $(CPP_FLAGS)
export rcpp_flags := $(RCPP_FLAGS)
export dcpp_flags := $(DCPP_FLAGS)
export common_cpp_flags := $(CPP_FLAGS)

export object_path := $(BUILD_PATH)/obj

LIQUID_ENGINE_FLAGS := $(c_flags) $(cpp_flags) $(link_flags) -Iliquid_engine
LIQUID_ENGINE_FLAGS += -DLD_EXPORT -MF $(object_path)/$(LIQUID_NAME).d

ifeq ($(IS_WINDOWS), true)
	LIQUID_ENGINE_FLAGS += -Wl,--out-implib=$(BUILD_PATH)/$(EXE_NAME).lib

	LIQUID_COMPILE_FILE := liquid_engine/platform/win32.cpp
	LIQUID_RESOURCES_PATH := win32/resources.rc
	LIQUID_RESOURCES_FILE := $(object_path)/win32_resources.o
endif

ifeq ($(IS_LINUX), true)
	LIQUID_ENGINE_FLAGS += -Wl,--out-implib=$(BUILD_PATH)/$(EXE_NAME).a

	LIQUID_COMPILE_FILE := liquid_engine/platform/linux.cpp
endif

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

deps := $(call recurse,$(object_path),*.d)

cpp := $(call recurse,liquid_engine/core/,*.cpp) $(call recurse,liquid_engine/renderer/,*.cpp)
h   := $(call recurse,liquid_engine,*.h)

corecpp := $(cpp)
corecpp := $(subst liquid_engine/,,$(corecpp))
corecpp := $(addsuffix \",$(corecpp))
corecpp := $(addprefix "#include \"",$(corecpp))

corecpp_path := liquid_engine/platform/corecpp.inl

all: print_info shader $(EXE_PATH)
	@$(MAKE) --directory=testbed --no-print-directory

print_info:
	@echo "Make: compilation target:" $(HOST_OS_NAME)-$(TARGET_ARCH)-$(if $(RELEASE),release,debug)

shader:
	@$(MAKE) --directory=shader --no-print-directory

$(corecpp_path): $(cpp)
	@echo "// * Description:     Includes all cpp files" > $(corecpp_path)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(corecpp_path)
	@echo "// * File Generated:  "$(shell date) >> $(corecpp_path)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(corecpp_path)
	@echo "" >> $(corecpp_path)
	for i in $(corecpp); do echo $$i >> $(corecpp_path); done

$(EXE_PATH): $(corecpp_path) $(if $(IS_WINDOWS),$(LIQUID_RESOURCES_FILE),)
	@echo "Make:    compiling" $(EXE_NAME)$(EXE_EXT) ". . ."
	@mkdir -p $(object_path)
	@$(CC) $(LIQUID_COMPILE_FILE) $(LIQUID_RESOURCES_FILE) -o $(EXE_PATH) $(LIQUID_ENGINE_FLAGS)

$(LIQUID_RESOURCES_FILE): $(LIQUID_RESOURCES_PATH)
	@echo "Make:    compiling" $(LIQUID_RESOURCES_FILE) ". . ."
	@mkdir -p $(object_path)
	@windres $(LIQUID_RESOURCES_PATH) -o $(LIQUID_RESOURCES_FILE)

# TODO(alicia): update this when project museum is being worked on
run: all
	@echo "Make: project museum is not yet ready :("

test: all
	@echo "Make: running test bed . . ."
	@cd $(BUILD_PATH) && ./$(EXE_NAME)$(EXE_EXT) --libload=$(TESTBED_NAME) --gl

# for debugging variables
spit:
	@echo $(LIQUID_ENGINE_FLAGS)
	@$(MAKE) --directory=testbed spit

# @$(MAKE) --directory=shader spit
 
help:
	@echo Usage: make [argument]
	@echo "  all:    compile everything"
	@echo "  run:    compile and run \"Project Museum\""
	@echo "  test:   compile and run \"Testbed\""
	@echo "  shader: compile shaders only"
	@echo "  clean:  delete everything in build directory"
	@echo "  help:   display this message"

clean:
	@echo Make: removing everything from build directory . . .
	@rm -r -f build/debug/*
	@rm -r -f build/release/*
	@rm -r -f resources/shaders/*
	@rm $(corecpp_path)

.PHONY: all run test spit help clean gencpp shader print_info

-include $(deps)

