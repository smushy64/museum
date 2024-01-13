# Description:  Makefile for Liquid Package Utility
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 06, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

OUTPUT_HASHES_PATH := $(GENERATED_PATH)/package_hashes.h
INPUT_HASHES_PATH := ./package_hashes.txt

TARGET := $(BUILD_PATH)/$(UTIL_PKG)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)
LOCAL_CPPFLAGS += -DLD_HEADLESS -DLD_CONSOLE_APP -DLD_APPLICATION_STATIC

CPPFLAGS := $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

INCLUDE := $(INCLUDE_FLAGS)

LOCAL_LDFLAGS := -L$(BUILD_PATH) -l$(LIB_CORE_NAME)

LDFLAGS := $(LOCAL_LDFLAGS) $(LINKER_FLAGS)

MAIN := main.c

C := $(call recurse,,*.c)
H := $(call recurse,,*.h)

GENERATED_DEP := $(filter-out $(MAIN),$(C))
GENERATED_DEP := $(addsuffix \",$(GENERATED_DEP))
GENERATED_DEP := $(addprefix "#include \"package/",$(GENERATED_DEP))

all: $(TARGET)

clean:
	@echo "Make: cleaning package generated hashes . . ."
	-@rm -f $(OUTPUT_HASHES_PATH) 2> /dev/null || true

config:
	@echo
	@echo "-------- util pkg ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    include"
	@echo
	@echo "ldflags:    "$(LOCAL_LDFLAGS)", linker"

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for util pkg . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

clean_dep:
	@echo "Make: cleaning util pkg dependencies . . ."
	-@rm -f $(GENERATED_DEP_PATH) 2> /dev/null || true

generate_compile_flags:
	@echo "Make: generating package "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

$(OUTPUT_HASHES_PATH): $(INPUT_HASHES_PATH) $(BUILD_PATH)/$(UTIL_HASH)
	@echo "Make: "$(shell $(BUILD_PATH)/$(UTIL_HASH) --output-path $(OUTPUT_HASHES_PATH) --file $(INPUT_HASHES_PATH) )

$(TARGET): $(GENERATED_DEP_PATH) $(OUTPUT_HASHES_PATH) $(C) $(H) $(DEP_CORE_H) $(DEP_SHARED_H) $(PLATFORM_DEP_C)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) -xc $(PLATFORM_DEP_C) $(MAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONY: all config generate_compile_flags clean clean_dep generate_hashes

