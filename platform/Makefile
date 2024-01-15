# Description:  Makefile for Liquid Platform Layer
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 27, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS_RELEASE :=
LOCAL_CPPFLAGS_DEBUG   := -DLD_CONSOLE_APP

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

all:

generate_compile_flags:
	@echo "Make: generating platform "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all generate_compile_flags

