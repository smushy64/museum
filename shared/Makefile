# Description:  Liquid Engine Core Library Makefile
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 03, 2023

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
CFLAGS += -Wno-gnu-empty-initializer -Wno-c2x-extensions

ifeq ($(RELEASE), true)
	CFLAGS += -O2 -ffast-math
else
	CFLAGS += -O0 -g
endif

ifeq ($(TARGET_ARCH), x86_64)
	CFLAGS += -masm=intel -msse4.2
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
CPPFLAGS += -DCORE_EXPORT -DSTACK_SIZE=$(LD_STACK_SIZE)

ifeq ($(RELEASE), true)
else
	CPPFLAGS += -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING
	CPPFLAGS += -DLD_DEVELOPER_MODE
endif

LDFLAGS := -shared

ifeq ($(TARGET_PLATFORM), win32)
	LDFLAGS += -fuse-ld=lld -nostdlib -lkernel32
	LDFLAGS += -mstack-probe-size=999999999 -Wl,//stack:$(LD_STACK_SIZE)
	LDFLAGS += -Wl,--out-implib=$(LOCAL_BUILD_PATH)/$(LIB_CORE_NAME).lib

	ifeq ($(RELEASE), true)
		LDFLAGS += -Wl,//release
	else
		LDFLAGS += -Wl,//debug
	endif
endif

INCLUDE := -I.

TARGET := $(LOCAL_BUILD_PATH)/$(LIB_CORE)

C := $(call recurse,,*.c)
H := $(call recurse,,*.h)

GENERATED_DEPENDENCIES := $(C)
GENERATED_DEPENDENCIES := $(addsuffix \",$(GENERATED_DEPENDENCIES))
GENERATED_DEPENDENCIES := $(addprefix "#include \"",$(GENERATED_DEPENDENCIES))
GENERATED_DEPENDENCIES_PATH := core_generated_dependencies.inl

all: $(TARGET)

generate_dependencies: $(GENERATED_DEPENDENCIES_PATH)

clean_dependencies:
	@echo "Make: cleaning liquid engine core dependencies . . ."
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

$(GENERATED_DEPENDENCIES_PATH): $(C)
	@echo "Make: generating dependencies for liquid engine core library . . ."
	@echo "// * Description:  Generated file containing all Liquid Engine core dependencies" > $(GENERATED_DEPENDENCIES_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "" >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in Liquid Engine Core and it should only be included ONCE." >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "" >> $(GENERATED_DEPENDENCIES_PATH)
	for i in $(GENERATED_DEPENDENCIES); do echo $$i >> $(GENERATED_DEPENDENCIES_PATH); done

$(TARGET): $(GENERATED_DEPENDENCIES_PATH) $(C) $(H)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(LOCAL_OBJ_PATH)
	@$(CC) $(CSTD) -xc $(GENERATED_DEPENDENCIES_PATH) ../liquid_platform/platform_dllmain.c -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all spit clean_dependencies generate_dependencies

