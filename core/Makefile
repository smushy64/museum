# Description:  Liquid Engine Core Library Makefile
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 03, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET      := $(BUILD_PATH)/$(LIB_CORE)
TEST_TARGET := $(BUILD_PATH)/$(LIB_CORE_NAME)-test

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS_LINUX := -D_LARGEFILE64_SOURCE

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4
LOCAL_CPPFLAGS += -DCORE_EXPORT -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)

ifeq ($(TARGET_PLATFORM), linux)
	LOCAL_CPPFLAGS += $(LOCAL_CPPFLAGS_LINUX)
endif

CPPFLAGS := $(LOCAL_CPPFLAGS) $(DEVELOPER_FLAGS)

INCLUDE := $(INCLUDE_FLAGS)

LOCAL_LDFLAGS := -shared

ifeq ($(TARGET_PLATFORM), win32)
	LOCAL_LDFLAGS += -Wl,--out-implib=$(BUILD_PATH)/$(LIB_CORE_NAME).lib
endif

LDFLAGS := $(LINKER_FLAGS_PRELUDE) $(LOCAL_LDFLAGS) $(LINKER_FLAGS_PLATFORM)

TEST_FILE := ../core/test.c

C := $(filter-out $(TEST_FILE),$(DEP_CORE_C))
H := $(DEP_CORE_H)

GENERATED_DEP := $(addsuffix \",$(C))
GENERATED_DEP := $(addprefix "#include \"",$(GENERATED_DEP))

all: $(TEST_TARGET)

clean_dep:
	@echo "Make: cleaning lib core dependencies . . ."
	@rm -f $(GENERATED_DEP_PATH) 2> /dev/null || true

config:
	@echo
	@echo "-------- lib core ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    include"
	@echo
	@echo "ldflags:    linker_prelude, "$(LOCAL_LDFLAGS)", linker_platform"
	@echo "c:          "$(C)

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for lib core . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

$(TARGET) : $(GENERATED_DEP_PATH) $(C) $(H)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) -xc $(GENERATED_DEP_PATH) $(DLLMAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

$(TEST_TARGET): $(TARGET) $(TEST_FILE)
	@echo "Make: testing "$(TARGET)" . . ."
	@$(CC) $(CSTD) -xc $(TEST_FILE) -o $(TEST_TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -L$(BUILD_PATH) -l$(LIB_CORE_NAME)
	@echo -n "Make: core: "
	$(TEST_TARGET)

generate_compile_flags:
	@echo "Make: generating core "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config clean_dep generate_compile_flags

