# * Description:  Testbed build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

LOCAL_BUILD_PATH := ../$(BUILD_PATH)
LOCAL_OBJ_PATH   := ../$(OBJ_PATH)

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

CXXFLAGS := -Werror -Wall -Wextra -pedantic -Werror=vla
CXXFLAGS += -fno-strict-enums -Wno-missing-braces
CXXFLAGS += -Wno-c11-extensions -Wno-gnu-zero-variadic-macro-arguments
CXXFLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types
CXXFLAGS += -Wno-ignored-attributes -Wno-gnu-case-range
CXXFLAGS += -Wno-fixed-enum-extension -Wno-static-in-inline
CXXFLAGS += -Wno-c99-extensions -Wno-duplicate-decl-specifier
CXXFLAGS += -Wno-gnu-empty-initializer

ifeq ($(RELEASE), true)
	CXXFLAGS += -O2 -ffast-math
else
	CXXFLAGS += -O0 -g
endif

ifeq ($(TARGET_ARCH), x86_64)
	CXXFLAGS += -masm=intel -mcpu=x86-64
else
	CXXFLAGS += -mcpu=$(TARGET_ARCH)
endif

ifeq ($(TARGET_PLATFORM), win32)
	ifeq ($(RELEASE), true)
	else
		CXXFLAGS += -gcodeview
	endif
endif

CPPFLAGS := -DLD_SIMD_WIDTH=4 -DSTACK_SIZE=$(LD_STACK_SIZE)

ifeq ($(RELEASE), true)
else
	CPPFLAGS += -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING
endif

LDFLAGS := -shared

ifeq ($(TARGET_PLATFORM), win32)
	LDFLAGS += -fuse-ld=lld -nostdlib
	LDFLAGS += -L$(LOCAL_BUILD_PATH) -l$(LIB_CORE_NAME)
	LDFLAGS += -mstack-probe-size=999999999 -Wl,//stack:$(LD_STACK_SIZE)

	ifeq ($(RELEASE), true)
		LDFLAGS += -Wl,//release
	else
		LDFLAGS += -Wl,//debug
	endif
endif

DLLMAIN     := ../liquid_platform/platform_dllmain.c
TESTBEDMAIN := entry.cpp

INCLUDE := -I. -I../liquid_engine

TARGET := $(LOCAL_BUILD_PATH)/$(LIB_TESTBED)

CPP := $(call recurse,,*.c)
CPP := $(filter-out $(COREMAIN),$(CPP))

H := $(call recurse,../liquid_engine,*.h)

GENERATED_DEPENDENCIES := $(CPP)
GENERATED_DEPENDENCIES := $(addsuffix \",$(GENERATED_DEPENDENCIES))
GENERATED_DEPENDENCIES := $(addprefix "#include \"",$(GENERATED_DEPENDENCIES))
GENERATED_DEPENDENCIES_PATH := core_generated_dependencies.inl

all: $(TARGET)

spit:
	@echo
	@echo "-------- lib testbed ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     "$(CXXFLAGS)
	@echo
	@echo "cppflags:   "$(CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    "$(LDFLAGS)
	@echo
	@echo "main:       "$(TESTBEDMAIN)

$(GENERATED_DEPENDENCIES_PATH): $(CPP)
	@echo "Make: generating dependencies for testbed . . ."
	@echo "// * Description:  Generated file containing all Liquid Engine testbed dependencies" > $(GENERATED_DEPENDENCIES_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "" >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(GENERATED_DEPENDENCIES_PATH)
	@echo "" >> $(GENERATED_DEPENDENCIES_PATH)
	for i in $(GENERATED_DEPENDENCIES); do echo $$i >> $(GENERATED_DEPENDENCIES_PATH); done

$(TARGET): $(GENERATED_DEPENDENCIES_PATH) $(TESTBEDMAIN) $(CPP) $(H)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(LOCAL_OBJ_PATH)
	@$(CXX) $(CXXSTD) -x c++ $(TESTBEDMAIN) ../liquid_platform/platform_dllmain.c -o $(TARGET) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all spit

