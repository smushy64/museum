# * Description:  Makefile for Project Museum
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023

# silent make output
MAKEFLAGS += -s
MAKEFLAGS += -j

export CC := clang++ -std=c++20
ENGINE_CC := clang -std=c99

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
export LIQUID_VERSION_MINOR := 2
export LIQUID_VERSION       := $(LIQUID_VERSION_MAJOR).$(LIQUID_VERSION_MINOR)
export LIQUID_NAME          := liquid-engine
export LIQUID_VERSION_PATH  := $(subst .,-,$(LIQUID_VERSION))

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

export EXE_NAME := $(LIQUID_NAME)-$(LIQUID_VERSION_PATH)-$(if $(IS_DEBUG),debug-,)$(HOST_OS_NAME)
export EXE_PATH := $(BUILD_PATH)/$(EXE_NAME)$(EXE_EXT)

export MUSEUM_NAME  := museum-$(if $(IS_DEBUG),debug,release)$(SO_EXT)
export TESTBED_NAME := testbed-$(if $(IS_DEBUG),debug,release)$(SO_EXT)

RC_FLAGS := -O2 -g -Werror -Wall -Wextra
DC_FLAGS := -O0 -g -Werror -Wall -Wextra -pedantic

C_FLAGS := -fno-rtti -fno-exceptions -Werror=vla -ffast-math
C_FLAGS += -fno-operator-names -fno-strict-enums
C_FLAGS += -MMD -MP
C_FLAGS += -Wno-missing-braces -Wno-c11-extensions
C_FLAGS += -Wno-gnu-zero-variadic-macro-arguments
C_FLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types
C_FLAGS += -Wno-unused-variable -Wno-ignored-attributes
C_FLAGS += -Wno-gnu-case-range -Wno-incompatible-library-redeclaration
C_FLAGS += -Wno-fixed-enum-extension -Wno-strict-prototypes
C_FLAGS += -Wno-gnu-empty-initializer -Wno-static-in-inline
C_FLAGS += -Wno-c99-extensions

ifeq ($(IS_WINDOWS), true)
	DC_FLAGS += -gcodeview
	RC_FLAGS += -gcodeview -Wno-unused-value -mwindows
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
CPP_FLAGS += -DLIQUID_ENGINE_EXECUTABLE=\"$(EXE_NAME)$(EXE_EXT)"\""
CPP_FLAGS += -DGL_VERSION_MAJOR=$(GL_VERSION_MAJOR)
CPP_FLAGS += -DGL_VERSION_MINOR=$(GL_VERSION_MINOR)
CPP_FLAGS += -DVULKAN_VERSION_MAJOR=$(VULKAN_VERSION_MAJOR)
CPP_FLAGS += -DVULKAN_VERSION_MINOR=$(VULKAN_VERSION_MINOR)

RLINK_FLAGS :=
DLINK_FLAGS :=
LINK_FLAGS  := 

ifeq ($(IS_WINDOWS), true)
	LINK_FLAGS  += -fuse-ld=lld
	DLINK_FLAGS += -Wl,//debug
	RLINK_FLAGS += -Wl,//debug

	LINK_FLAGS  += -nostdlib
	LINK_FLAGS  += -lkernel32 -mstack-probe-size=999999999 -Wl,//stack:0x100000
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

LIQUID_ENGINE_FLAGS := $(c_flags) $(cpp_flags) $(link_flags) -Iliquid_engine -Ivendor
LIQUID_ENGINE_FLAGS += -DLD_EXPORT -MF $(object_path)/$(LIQUID_NAME).d

ifeq ($(IS_WINDOWS), true)
	LIQUID_ENGINE_FLAGS += -Wl,--out-implib=$(BUILD_PATH)/$(EXE_NAME).lib

	LIQUID_COMPILE_FILE   := liquid_engine/platform/win32.c
	LIQUID_RESOURCES_PATH := win32/resources.rc
	LIQUID_RESOURCES_FILE := $(object_path)/win32_resources.o
endif

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

deps := $(call recurse,$(object_path),*.d)

c := $(call recurse,liquid_engine/core/,*.c) $(call recurse,liquid_engine/renderer/,*.c)
h := $(call recurse,liquid_engine,*.h)

corec := $(c)
corec := $(subst liquid_engine/,,$(corec))
corec := $(addsuffix \",$(corec))
corec := $(addprefix "#include \"",$(corec))

corec_path := liquid_engine/platform/corec.inl

all: print_info shaders resources $(EXE_PATH)
	@$(MAKE) --directory=testbed --no-print-directory

print_info:
	@echo "Make: compilation target:" $(HOST_OS_NAME)-$(TARGET_ARCH)-$(if $(RELEASE),release,debug)

lepkg:
	@$(MAKE) --directory=lepkg --no-print-directory

resources: shaders
	@$(MAKE) --directory=resources --no-print-directory

shaders:
	@$(MAKE) --directory=shaders --no-print-directory

$(corec_path): $(c)
	@echo "// * Description:     Includes all source files" > $(corec_path)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(corec_path)
	@echo "// * File Generated:  "$(shell date) >> $(corec_path)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(corec_path)
	@echo "" >> $(corec_path)
	for i in $(corec); do echo $$i >> $(corec_path); done

$(EXE_PATH): $(corec_path) $(if $(IS_WINDOWS),$(LIQUID_RESOURCES_FILE),)
	@echo "Make: compiling" $(EXE_NAME)$(EXE_EXT) ". . ."
	@mkdir -p $(object_path)
	@$(ENGINE_CC) $(LIQUID_COMPILE_FILE) $(LIQUID_RESOURCES_FILE) -o $(EXE_PATH) $(LIQUID_ENGINE_FLAGS)

$(LIQUID_RESOURCES_FILE): $(LIQUID_RESOURCES_PATH)
	@echo "Make: compiling" $(LIQUID_RESOURCES_FILE) ". . ."
	@mkdir -p $(object_path)
	@windres $(LIQUID_RESOURCES_PATH) -o $(LIQUID_RESOURCES_FILE)

# TODO(alicia): update this when project museum is being worked on
run: all
	@echo "Make: project museum is not yet ready :("

test: all
	@echo "Make: running test bed . . ."
	@cd $(BUILD_PATH) && ./$(EXE_NAME)$(EXE_EXT) --libload=$(TESTBED_NAME) --gl

pack:
	@$(MAKE) --directory=lepkg run

# for debugging variables
spit:
	@echo $(ENGINE_CC) $(LIQUID_COMPILE_FILE) $(LIQUID_RESOURCES_FILE) -o $(EXE_PATH) $(LIQUID_ENGINE_FLAGS)

# @$(MAKE) --directory=shader spit
 
help:
	@echo "Usage: make [argument]"
	@echo ""
	@echo "Arguments:"
	@echo "  all:    compile everything"
	@echo "  run:    compile and run \"Project Museum\""
	@echo "  test:   compile and run \"Testbed\""
	@echo "  shader: compile shaders only"
	@echo "  help:   display this message"
	@echo "  clean:  delete everything in build directory"
	@echo "  cleanr: delete resources only"

clean: print_clean_info cleanr
	@rm -r -f build/debug/*
	@rm -r -f build/release/*
	@rm -r -f resources/shaders
	@rm $(corec_path)

print_clean_info:
	@echo "Make: removing everything from build directory . . ."

cleanr:
	@$(MAKE) --directory=resources clean

.PHONY: all run test spit help clean shaders print_info resources cleanr print_clean_info lepkg pack

-include $(deps)

