# * Description:  Liquid Engine Core Library Makefile
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 23, 2023

LOCAL_BUILD_PATH  := ../$(BUILD_PATH)
LOCAL_OBJ_PATH    := ../$(OBJ_PATH)

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

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
	else
		CFLAGS += -gcodeview
	endif
endif

CPPFLAGS := -DLD_SIMD_WIDTH=4
CPPFLAGS += -DLIQUID_ENGINE_VERSION=\""$(LD_NAME) $(LD_VERSION)"\"
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MAJOR=$(LD_MAJOR)
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MINOR=$(LD_MINOR)
CPPFLAGS += -DLIQUID_ENGINE_EXECUTABLE=\"$(LD_EXE_NAME)$(if $(EXE_EXT),.$(EXE_EXT),)\"
CPPFLAGS += -DGL_VERSION_MAJOR=$(GL_MAJOR)
CPPFLAGS += -DGL_VERSION_MINOR=$(GL_MINOR)
CPPFLAGS += -DVULKAN_VERSION_MAJOR=$(VK_MAJOR)
CPPFLAGS += -DVULKAN_VERSION_MINOR=$(VK_MINOR)
CPPFLAGS += -DLD_EXPORT -DSTACK_SIZE=$(LD_STACK_SIZE)

ifeq ($(RELEASE), true)
else
	CPPFLAGS += -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING
	CPPFLAGS += -DLD_DEVELOPER_MODE
endif

LDFLAGS := -shared

ifeq ($(TARGET_PLATFORM), win32)
	LDFLAGS += -fuse-ld=lld -nostdlib -lkernel32
	LDFLAGS += -mstack-probe-size=999999999 -Wl,//stack:$(LD_STACK_SIZE)
	# LDFLAGS += -Wl,--out-implib=$(LOCAL_BUILD_PATH)/$(LIB_CORE_NAME).lib

	ifeq ($(RELEASE), true)
		LDFLAGS += -Wl,//release
	else
		LDFLAGS += -Wl,//debug
	endif
endif

COREMAIN := core/main.c

INCLUDE := -I. -I../liquid_platform

TARGET := $(LOCAL_BUILD_PATH)/$(LIB_CORE)

C := $(call recurse,,*.c)
C := $(filter-out $(COREMAIN),$(C))

H := $(call recurse,,*.h)

GENERATED_DEPENDENCIES := $(C)
GENERATED_DEPENDENCIES := $(addsuffix \",$(GENERATED_DEPENDENCIES))
GENERATED_DEPENDENCIES := $(addprefix "#include \"",$(GENERATED_DEPENDENCIES))
GENERATED_DEPENDENCIES_PATH := core_generated_dependencies.inl

all: $(TARGET)

generate_dependencies: $(GENERATED_DEPENDENCIES_PATH)

clean_dependencies:
	@echo "Make: cleaning liquid engine dependencies . . ."
	-@rm -f $(GENERATED_DEPENDENCIES_PATH) 2> /dev/null || true

spit:
	@echo
	@echo "-------- lib core ---------"
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
	@echo "main:       "$(COREMAIN)

$(GENERATED_DEPENDENCIES_PATH): $(C)
	@echo "Make: generating dependencies for liquid engine . . ."
	@echo "// * Description:  Generated file containing all Liquid Engine core dependencies" > $(GENERATED_DEPENDENCIES_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "" >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in Liquid Engine Core and it should only be included ONCE." >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "" >> $(GENERATED_DEPENDENCIES_PATH)
	for i in $(GENERATED_DEPENDENCIES); do echo $$i >> $(GENERATED_DEPENDENCIES_PATH); done

$(TARGET): $(GENERATED_DEPENDENCIES_PATH) $(COREMAIN) $(C) $(H) ../liquid_platform/platform.h
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(LOCAL_OBJ_PATH)
	@$(CC) $(CSTD) $(COREMAIN) ../liquid_platform/platform_dllmain.c -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all spit clean_dependencies generate_dependencies

