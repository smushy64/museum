# Description:  Makefile for String Hash Generator.
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 24, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(UTIL_HASH_FILE)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)
LOCAL_CPPFLAGS += -DLD_CONSOLE_APP -DLD_HEADLESS -DLD_APPLICATION_STATIC

CPPFLAGS := $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

INCLUDE := $(INCLUDE_FLAGS)

LOCAL_LDFLAGS := -L$(BUILD_PATH) -l$(LIB_CORE_NAME) 
ifeq ($(TARGET_PLATFORM), linux)
	LOCAL_LDFLAGS += -Wl,-rpath,'$$ORIGIN'
endif

LDFLAGS := $(LOCAL_LDFLAGS) $(LINKER_FLAGS)

MAIN := main.c

all: $(TARGET)

config:
	@echo
	@echo "-------- util hash ---------"
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
	@echo "main:       "$(MAIN)

generate_compile_flags:
	@echo "Make: generating hash "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

$(TARGET): $(MAIN) $(DEP_CORE_H) $(DEP_SHARED_H) $(DEP_PLATFORM_C)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(DEP_PLATFORM_C) $(MAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all config generate_compile_flags clean_dep

