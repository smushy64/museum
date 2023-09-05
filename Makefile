# * Description:  Project build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

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

	LDMAIN := liquid_engine/platform/ldwin32main.c
else
	export EXE_EXT :=
	export SO_EXT  := so

	ifeq ($(TARGET_PLATFORM), linux)
		LDMAIN := liquid_engine/platform/ldlinuxmain.c
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

export LD_EXE_NAME := $(LD_NAME)-$(LD_VERSION_PATH)-$(TARGET_PLATFORM)-$(TARGET_ARCH)$(if $(RELEASE),,-debug)
export TARGET      := $(BUILD_PATH)/$(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)

CPPFLAGS := -DLD_SIMD_WIDTH=4
CPPFLAGS += -DLIQUID_ENGINE_VERSION=\""$(LD_NAME) $(LD_VERSION)"\"
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MAJOR=$(LD_MAJOR)
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MINOR=$(LD_MINOR)
CPPFLAGS += -DLIQUID_ENGINE_EXECUTABLE=\"$(TARGET)\"
CPPFLAGS += -DGL_VERSION_MAJOR=$(GL_MAJOR)
CPPFLAGS += -DGL_VERSION_MINOR=$(GL_MINOR)
CPPFLAGS += -DVULKAN_VERSION_MAJOR=$(VK_MAJOR)
CPPFLAGS += -DVULKAN_VERSION_MINOR=$(VK_MINOR)
CPPFLAGS += -DLD_EXPORT

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

	LDFLAGS += -Wl,--out-implib=$(BUILD_PATH)/$(LD_EXE_NAME).lib
endif

ifeq ($(TARGET_PLATFORM), linux)
	LDFLAGS += -fPIC -pie -Wl,-E
	LDFLAGS += $(shell pkg-config --libs sdl2)
	LDFLAGS += -lpthread
endif

INCLUDE := -Iliquid_engine -Ivendor

ifeq ($(TARGET_PLATFORM), linux)
	INCLUDE += $(shell pkg-config --cflags-only-I sdl2)
endif

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

dependencies := $(call recurse,$(OBJ_PATH),*.d)

C := $(call recurse,liquid_engine/core/,*.c) $(call recurse,liquid_engine/ldrenderer/,*.c)
H := $(call recurse,liquid_engine,*.h)

COREC := $(C)
COREC := $(subst liquid_engine/,,$(COREC))
COREC := $(addsuffix \",$(COREC))
COREC := $(addprefix "#include \"",$(COREC))

COREC_PATH := liquid_engine/platform/corec.inl

all: shaders $(if $(SHADER_ONLY),,$(TARGET))

test: build_testbed
	@mkdir -p $(BUILD_PATH)/resources/shaders
	@cp resources/shaders/ldcolor.vert.spv $(BUILD_PATH)/resources/shaders/ldcolor.vert.spv
	@cp resources/shaders/ldcolor.frag.spv $(BUILD_PATH)/resources/shaders/ldcolor.frag.spv
	@cd $(BUILD_PATH) && ./$(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),) --libload=$(LIB_TESTBED) --gl

build_testbed: all
	@$(MAKE) --directory=testbed --no-print-directory

shaders:
	@$(MAKE) --directory=shaders --no-print-directory

run:
	@echo run none

spit:
	@echo "platform:   "$(TARGET_PLATFORM)
	@echo "arch:       "$(TARGET_ARCH)
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
	@echo "corec:      "$(COREC)
	@echo
	@echo "main:       "$(LDMAIN)
	@$(MAKE) --directory=testbed spit
	@$(MAKE) --directory=shaders spit

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

$(COREC_PATH): $(C)
	@echo "// * Description:     Includes all source files" > $(COREC_PATH)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(COREC_PATH)
	@echo "// * File Generated:  "$(shell date) >> $(COREC_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(COREC_PATH)
	@echo "" >> $(COREC_PATH)
	for i in $(COREC); do echo $$i >> $(COREC_PATH); done

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

