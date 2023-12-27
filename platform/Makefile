# Description:  Makefile for Liquid Platform Layer
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 27, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(EXE)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS_RELEASE :=
LOCAL_CPPFLAGS_DEBUG := -DLD_CONSOLE_APP

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DLD_EXPORT
LOCAL_CPPFLAGS += -DLD_PLATFORM_INTERNAL

ifeq ($(RELEASE), true)
	LOCAL_CPPFLAGS += $(LOCAL_CPPFLAGS_RELEASE)
else
	LOCAL_CPPFLAGS += $(LOCAL_CPPFLAGS_DEBUG)
endif

CPPFLAGS := $(ENGINE_FLAGS) $(GRAPHICS_FLAGS) $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

LDFLAGS := $(LINKER_FLAGS)

INCLUDE := $(INCLUDE_FLAGS)

ifeq ($(TARGET_PLATFORM), win32)
	IS_WINDOWS := true
	RESOURCES_WIN32 := $(OBJ_PATH)/win32_resources.o
	WIN32_RC_PATH   := ../platform/win32/resources.rc
endif

C := $(PLATFORM_DEP_C)
H := $(PLATFORM_DEP_H)

all: $(TARGET)

config:
	@echo
	@echo "-------- exe ---------------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   engine, graphics, developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    include"
	@echo
	@echo "ldflags:    linker"

$(RESOURCES_WIN32): $(WIN32_RC_PATH)
	@echo "Make: compiling "$(RESOURCES_WIN32)". . ."
	@mkdir -p $(OBJ_PATH)
	@windres $(WIN32_RC_PATH) -o $(RESOURCES_WIN32)

$(TARGET): $(if $(IS_WINDOWS),$(RESOURCES_WIN32),) $(C) $(H) $(DEP_SHARED_H) $(DEP_SHARED_C)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(C) $(RESOURCES_WIN32) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

generate_compile_flags:
	@echo "Make: generating platform "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(CFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config generate_compile_flags

