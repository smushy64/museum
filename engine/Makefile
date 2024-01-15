# * Description:  Liquid Engine Library Makefile
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 23, 2023

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := $(BUILD_PATH)/$(ENGINE_FILE)

CFLAGS := $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS)

LOCAL_CPPFLAGS := -DLD_SIMD_WIDTH=4 -DLD_EXPORT

ifeq ($(RELEASE),true)
else
	LOCAL_CPPFLAGS += -DLD_CONSOLE_APP
endif

CPPFLAGS := $(ENGINE_FLAGS) $(GRAPHICS_FLAGS) $(DEVELOPER_FLAGS) $(LOCAL_CPPFLAGS)

LOCAL_LDFLAGS := -L$(BUILD_PATH) -l$(LIB_CORE_NAME) -l$(LIB_MEDIA_NAME)

ifeq ($(TARGET_PLATFORM), win32)
	RESOURCES_WIN32     := $(OBJ_PATH)/resources.o
	RESOURCES_WIN32_SRC := ../platform/win32/resources.rc
	LOCAL_LDFLAGS       += -Wl,--out-implib=$(BUILD_PATH)/$(subst exe,lib,$(ENGINE_FILE))
endif

ifeq ($(TARGET_PLATFORM), linux)
	LOCAL_LDFLAGS += -shared -Wl,-e,_start -Wl,-soname,$(ENGINE_FILE)
	LOCAL_LDFLAGS += -Wl,-rpath,'$$ORIGIN' -DSHARED_EXECUTABLE
	LOCAL_LDFLAGS += -Wl,--out-implib=$(BUILD_PATH)/$(subst so,a,$(ENGINE_FILE))
endif

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
	@echo "cflags:     "$(CFLAGS)
	@echo
	@echo "cppflags:   "$(CPPFLAGS)
	@echo
	@echo "include:    "$(INCLUDE)
	@echo
	@echo "ldflags:    "$(LDFLAGS)
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

$(RESOURCES_WIN32): $(RESOURCES_WIN32_SRC)
	@echo "Make: compiling "$(RESOURCES_WIN32)" . . ."
	@mkdir -p $(OBJ_PATH)
	@windres $(RESOURCES_WIN32_SRC) -o $(RESOURCES_WIN32)

$(TARGET): $(RESOURCES_WIN32) $(GENERATED_DEP_PATH) $(C) $(H) $(DEP_SHARED_H) $(DEP_SHARED_C) $(DEP_CORE_H) $(DEP_PLATFORM_C) $(DLLMAIN)
	@echo "Make: compiling "$(TARGET)" . . ."
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CSTD) $(RESOURCES_WIN32) $(DEP_PLATFORM_C) -xc $(MAIN) -o $(TARGET) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) 

generate_compile_flags:
	@echo "Make: generating engine "$(COMPILE_COMMANDS_PATH)". . ."
	@echo $(CC) > $(COMPILE_COMMANDS_PATH)
	@echo $(CSTD) >> $(COMPILE_COMMANDS_PATH)
	for i in $(filter-out -Werror -pedantic,$(CFLAGS)); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	for i in $(CPPFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done
	@echo "-I.." >> $(COMPILE_COMMANDS_PATH)
	for i in $(LDFLAGS); do echo $$i >> $(COMPILE_COMMANDS_PATH); done

.PHONY: all config clean_dep generate_compile_flags

