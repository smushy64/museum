# Description:  Makefile for String Hash Generator.
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 24, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

LOCAL_BUILD_PATH := ../$(BUILD_PATH)
LOCAL_OBJ_PATH   := ../$(OBJ_PATH)

TARGET := $(LOCAL_BUILD_PATH)/$(UTIL_HASH)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)
LOCAL_CPPFLAGS += -DLD_CONSOLE_APP -DLD_HEADLESS -DLD_CORE_STATIC_BUILD

CPPFLAGS := $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

INCLUDE := $(INCLUDE_FLAGS)

LOCAL_LDFLAGS := -L$(LOCAL_BUILD_PATH) -l$(LIB_CORE_NAME)

LDFLAGS := $(LOCAL_LDFLAGS) $(LINKER_FLAGS)

MAIN := main.c

C := $(call recurse,,*.c)
H := $(call recurse,,*.h)

GENERATED_DEP := $(filter-out $(MAIN),$(C))
GENERATED_DEP := $(addsuffix \",$(GENERATED_DEP))
GENERATED_DEP := $(addprefix "#include \"",$(GENERATED_DEP))

all: $(TARGET)

spit:
	@echo
	@echo "-------- util hash ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    include"
	@echo
	@echo "ldflags:    linker, "$(LOCAL_LDFLAGS)
	@echo
	@echo "main:       "$(MAIN)

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for util hash . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

COMPILE_COMMANDS_PATH := compile_flags.txt

generate_compile_flags:
	@echo "Make: generating hash "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(CFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

$(TARGET): $(GENERATED_DEP_PATH) $(C) $(H) $(DEP_CORE_H) $(DEP_SHARED_H) $(PLATFORM_DEP_C)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(LOCAL_OBJ_PATH)
	@$(CC) $(CSTD) -xc $(PLATFORM_DEP_C) $(MAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all spit generate_compile_flags

