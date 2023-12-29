# * Description:  Liquid Engine Library Makefile
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 23, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(LIB_ENGINE)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4 -DLD_EXPORT

CPPFLAGS := $(ENGINE_FLAGS) $(GRAPHICS_FLAGS) $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

LOCAL_LDFLAGS := -shared -L$(BUILD_PATH) -l$(LIB_CORE_NAME) -l$(LIB_MEDIA_NAME)

LDFLAGS := $(LOCAL_LDFLAGS) $(LINKER_FLAGS)

INCLUDE := $(INCLUDE_FLAGS)

C := $(call recurse,../engine,*.c)
H := $(call recurse,../engine,*.h)

MAIN := ../engine/main.c

GENERATED_DEP := $(filter-out $(MAIN),$(C))
GENERATED_DEP := $(addsuffix \",$(GENERATED_DEP))
GENERATED_DEP := $(addprefix "#include \"",$(GENERATED_DEP))

all: $(TARGET)

config:
	@echo
	@echo "-------- lib engine ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     warning, optimization, arch, platform"
	@echo
	@echo "cppflags:   engine, graphics, developer, "$(LOCAL_CPPFLAGS)
	@echo
	@echo "include:    include"
	@echo
	@echo "ldflags:    "$(LOCAL_LDFLAGS)", linker"
	@echo
	@echo "main:       "$(MAIN)

clean_dep:
	@echo "Make: cleaning lib engine dependencies . . ."
	-@rm -f $(GENERATED_DEPENDENCIES_PATH) 2> /dev/null || true

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for lib engine . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

$(TARGET): $(GENERATED_DEP_PATH) $(C) $(H) $(DEP_SHARED_H) $(DEP_SHARED_C) $(DEP_CORE_H) $(DEP_CORE_C)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) -xc $(DLLMAIN) $(MAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) 

generate_compile_flags:
	@echo "Make: generating engine "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config clean_dep generate_compile_flags

