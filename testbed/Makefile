# * Description:  Testbed build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(LIB_TESTBED)

CXXFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)

CPPFLAGS := $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

LOCAL_LDFLAGS := -shared -L$(BUILD_PATH) -l$(LIB_CORE_NAME) -l$(LIB_ENGINE_NAME)

LDFLAGS := $(LINKER_FLAGS_PRELUDE) $(LOCAL_LDFLAGS) $(LINKER_FLAGS_PLATFORM)

INCLUDE := $(INCLUDE_FLAGS)

MAIN := ../testbed/entry.cpp

C := $(call recurse,../testbed,*.cpp)
H := $(call recurse,../testbed,*.h)

GENERATED_DEP := $(filter-out $(MAIN),$(C))
GENERATED_DEP := $(addsuffix \",$(GENERATED_DEP))
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
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    include"
	@echo
	@echo "ldflags:    linker_prelude, "$(LOCAL_LDFLAGS)", linker_platform"
	@echo
	@echo "main:       "$(MAIN)

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for lib testbed . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

$(TARGET): $(GENERATED_DEP_PATH) $(C) $(H) $(DEP_SHARED_H) $(DEP_CORE_H)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CXX) $(CXXSTD) -x c++ $(MAIN) $(DLLMAIN) -o $(TARGET) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS)

generate_compile_flags:
	@echo "Make: generating testbed "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CXX) > $(COMPILE_COMMANDS_PATH)
	@echo $(CXXSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(CXXFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config clean_dep generate_compile_flags

