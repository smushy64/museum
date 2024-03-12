# Description:  Makefile for Unpack Utility.
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: January 17, 2024

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

OUTPUT_HASHES_PATH := $(GENERATED_PATH)/unpack_hashes.h
INPUT_HASHES_PATH  := ./unpack_hashes.txt

TARGET := $(BUILD_PATH)/$(UTIL_UNPACK_FILE)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)
LOCAL_CPPFLAGS += -DLD_CONSOLE_APP

CPPFLAGS := $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

INCLUDE := $(INCLUDE_FLAGS)

LOCAL_LDFLAGS := -L$(BUILD_PATH) -l$(LIB_CORE_NAME) -l$(LIB_MEDIA_NAME)
ifeq ($(TARGET_PLATFORM), linux)
	LOCAL_LDFLAGS += -Wl,-rpath,'$$ORIGIN'
endif

LDFLAGS := $(LOCAL_LDFLAGS) $(LINKER_FLAGS)

MAIN := main.c

C := $(call recurse,,*.c)
H := $(call recurse,,*.h)

GENERATED_DEP := $(filter-out $(MAIN),$(C))
GENERATED_DEP := $(addsuffix \",$(GENERATED_DEP))
GENERATED_DEP := $(addprefix "#include \"unpack/",$(GENERATED_DEP))

all: $(TARGET)

config:
	@echo
	@echo "-------- util unpack ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     "$(CFLAGS)
	@echo
	@echo "cppflags:   "$(CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    "$(LDFLAGS)

generate_compile_flags:
	@echo "Make: generating unpack "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for util unpack . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

clean_dep:
	@echo "Make: cleaning util unpack dependencies . . ."
	-@rm -f $(GENERATED_DEP_PATH) 2> /dev/null || true

$(OUTPUT_HASHES_PATH): $(INPUT_HASHES_PATH) $(BUILD_PATH)/$(UTIL_HASH_FILE)
	@echo "Make: "$(shell $(BUILD_PATH)/$(UTIL_HASH_FILE) --output-path $(OUTPUT_HASHES_PATH) --file $(INPUT_HASHES_PATH) )

$(TARGET): $(MAIN) $(C) $(H) $(DEP_CORE_H) $(DEP_SHARED_H) $(DEP_PLATFORM_C) $(GENERATED_DEP_PATH) $(OUTPUT_HASHES_PATH)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(DEP_PLATFORM_C) $(MAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all config generate_compile_flags clean_dep

