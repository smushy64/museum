# Description:  Liquid Media Layer
# Author:       Alicia Amarilla (smushyaa@gmail.com)
# File Created: December 28, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(LIB_MEDIA_FILE)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

XINPUT_POLL_RATE := 500

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4 -DMEDIA_EXPORT -DSTACK_SIZE=$(PROGRAM_STACK_SIZE)
LOCAL_CPPFLAGS += -DMEDIA_WIN32_XINPUT_POLL_RATE=$(XINPUT_POLL_RATE)

CPPFLAGS := $(GRAPHICS_FLAGS) $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

LOCAL_LDFLAGS := -shared -L$(BUILD_PATH) -l$(LIB_CORE_NAME)

ifeq ($(TARGET_PLATFORM), linux)
	DEPENDENCIES := $(shell pkg-config --libs sdl2)
	LOCAL_LDFLAGS := $(DEPENDENCIES) $(LOCAL_LDFLAGS) 
endif

LDFLAGS := $(LOCAL_LDFLAGS) $(LINKER_FLAGS)

INCLUDE := $(INCLUDE_FLAGS)

ifeq ($(TARGET_PLATFORM), linux)
	INCLUDE += $(shell pkg-config --cflags-only-I sdl2)
endif

C := $(call recurse,../media,*.c)
H := $(call recurse,../media,*.h)

GENERATED_DEP := $(addsuffix \",$(C))
GENERATED_DEP := $(addprefix "#include \"",$(GENERATED_DEP))

all: $(TARGET)

config:
	@echo
	@echo "-------- lib media ---------"
	@echo "target:     "$(TARGET)
	@echo
	@echo "cflags:     "$(CFLAGS)
	@echo
	@echo "cppflags:   "$(CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    "$(LDFLAGS)

clean_dep:
	@echo "Make: cleaning lib media dependencies . . ."
	-@rm -f $(GENERATED_DEPENDENCIES_PATH) 2> /dev/null || true

$(GENERATED_DEP_PATH): $(C)
	@echo "Make: generating dependencies for lib media . . ."
	@echo "// * Description:  Generated file containing dependencies" > $(GENERATED_DEP_PATH)
	@echo "// * Author:       Alicia Amarilla (smushyaa@gmail.com)" >> $(GENERATED_DEP_PATH)
	@echo "// * Generated on: "$(shell date) >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	@echo "// IMPORTANT(alicia): This file should only ever be included in the current directory and it should only be included ONCE." >> $(GENERATED_DEP_PATH)
	@echo "" >> $(GENERATED_DEP_PATH)
	for i in $(GENERATED_DEP); do echo $$i >> $(GENERATED_DEP_PATH); done

$(TARGET): $(GENERATED_DEP_PATH) $(C) $(H) $(DEP_SHARED_H) $(DEP_SHARED_C) $(DEP_CORE_H)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) -xc $(DLLMAIN) $(GENERATED_DEP_PATH) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) 

generate_compile_flags:
	@echo "Make: generating media "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config clean_dep generate_compile_flags

