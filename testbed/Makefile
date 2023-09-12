# * Description:  Testbed build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

TARGET := ../$(BUILD_PATH)/$(LIB_TESTBED)

TESTBEDMAIN := entry.cpp

dependencies := ../$(OBJ_PATH)/$(LIB_TESTBED_NAME).d

CXXFLAGS := -Werror -Wall -Wextra -pedantic -Werror=vla
CXXFLAGS += -fno-strict-enums -Wno-missing-braces
CXXFLAGS += -Wno-c11-extensions -Wno-gnu-zero-variadic-macro-arguments
CXXFLAGS += -Wno-gnu-anonymous-struct -Wno-nested-anon-types
CXXFLAGS += -Wno-ignored-attributes -Wno-gnu-case-range
CXXFLAGS += -Wno-fixed-enum-extension -Wno-static-in-inline
CXXFLAGS += -Wno-c99-extensions -Wno-duplicate-decl-specifier
CXXFLAGS += -Wno-gnu-empty-initializer
CXXFLAGS += -MMD -MP -MF $(dependencies)

ifeq ($(RELEASE), true)
	CXXFLAGS += -O2 -ffast-math
else
	CXXFLAGS += -O0 -g
endif

ifeq ($(TARGET_ARCH), x86_64)
	CXXFLAGS += -masm=intel
else
	CXXFLAGS += -mcpu=$(TARGET_ARCH)
endif

ifeq ($(TARGET_PLATFORM), win32)
	ifeq ($(RELEASE), true)
		CXXFLAGS += -mwindows
	else
		CXXFLAGS += -gcodeview
	endif
endif


CPPFLAGS := -DLD_SIMD_WIDTH=4
CPPFLAGS += -DLIQUID_ENGINE_VERSION=\""$(LD_NAME) $(LD_VERSION)"\"
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MAJOR=$(LD_MAJOR)
CPPFLAGS += -DLIQUID_ENGINE_VERSION_MINOR=$(LD_MINOR)

ifeq ($(RELEASE), true)
else
	CPPFLAGS += -DDEBUG -DLD_LOGGING -DLD_ASSERTIONS -DLD_PROFILING
endif

LDFLAGS :=
ifeq ($(TARGET_PLATFORM), win32)
	LDFLAGS += -fuse-ld=lld -nostdlib++
	LDFLAGS += -lmingw32 -shared -static-libgcc
	LDFLAGS += ../$(BUILD_PATH)/$(LD_EXE_NAME).$(EXE_EXT)

	ifeq ($(RELEASE), true)
		LDFLAGS += -Wl,//release
	else
		LDFLAGS += -Wl,//debug
	endif
endif

ifeq ($(TARGET_PLATFORM), linux)
	LDFLAGS += -shared -fPIC -L../$(BUILD_PATH)/$(LD_EXE_NAME)
endif

INCLUDE := -I../liquid_engine/ -I.

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

CPP := $(call recurse,,*.cpp)
CPP := $(filter-out $(TESTBEDMAIN),$(CPP))
HPP := $(call recurse,,*.h)

CORECPP := $(CPP)
CORECPP := $(addsuffix \",$(CORECPP))
CORECPP := $(addprefix "#include \"",$(CORECPP))

CORECPP_PATH := corecpp.inl

all: $(TARGET)

spit:
	@echo
	@echo "testbed target:   "$(TARGET)
	@echo "testbed compiler: "$(CXX)
	@echo "testbed standard: "$(CXXSTD)
	@echo "testbed cflags:   "$(CXXFLAGS)
	@echo
	@echo "testbed cppflags: "$(CPPFLAGS)
	@echo
	@echo "testbed include:  "$(INCLUDE)
	@echo
	@echo "testbed ldflags:  "$(LDFLAGS)
	@echo
	@echo "testbed main:     "$(TESTBEDMAIN)

$(CORECPP_PATH): $(CPP)
	@echo "// * Description:     Includes all CPP files" > $(CORECPP_PATH)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(CORECPP_PATH)
	@echo "// * File Generated:  "$(shell date) >> $(CORECPP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(CORECPP_PATH)
	@echo "" >> $(CORECPP_PATH)
	for i in $(CORECPP); do echo $$i >> $(CORECPP_PATH); done

$(TARGET): $(CORECPP_PATH)
	@echo "Make: compiling $(TARGET) . . ."
	@mkdir -p ../$(OBJ_PATH)
	@$(CXX) $(CXXSTD) $(TESTBEDMAIN) -o $(TARGET) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all spit

-include $(dependencies)

