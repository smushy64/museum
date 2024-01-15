# * Description:  Testbed build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(LIB_TESTBED_FILE)

CXXFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)

CPPFLAGS := $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

LOCAL_LDFLAGS := -shared -L$(BUILD_PATH) -l$(LIB_CORE_NAME)

ifeq ($(TARGET_PLATFORM), linux)
	LOCAL_LDFLAGS += -l:$(ENGINE_FILE) -Wl,-rpath,'$$ORIGIN'
else
	LOCAL_LDFLAGS += $(BUILD_PATH)/$(ENGINE_FILE)
endif

LDFLAGS := $(LINKER_FLAGS_PRELUDE) $(LOCAL_LDFLAGS) $(LINKER_FLAGS_PLATFORM)

INCLUDE := $(INCLUDE_FLAGS)

C := $(call recurse,../testbed,*.cpp)
H := $(call recurse,../testbed,*.h)

GENERATED_DEP := $(addsuffix \",$(C))
GENERATED_DEP := $(addprefix "#include \"",$(GENERATED_DEP))

all: $(TARGET)

clean_dep:
	@echo "Make: cleaning testbed dependencies . . ."
	-@rm -f $(GENERATED_DEP_PATH) 2> /dev/null || true

config:
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

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for lib testbed . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

$(TARGET): $(GENERATED_DEP_PATH) $(C) $(H) $(DEP_SHARED_H) $(DEP_CORE_H) $(DEP_ENGINE_H)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CXX) $(CXXSTD) -o $(TARGET) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) -xc++ $(GENERATED_DEP_PATH) $(DLLMAIN)

generate_compile_flags:
	@echo "Make: generating testbed "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CXX) > $(COMPILE_COMMANDS_PATH)
	@echo $(CXXSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CXXFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config clean_dep generate_compile_flags

