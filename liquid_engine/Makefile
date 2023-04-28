# * Description:  Makefile for Liquid Engine DLL
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023
# * Notes:        

MAKEFLAGS += -j5

target  := ../$(BUILD_PATH)/$(liquid_engine_dll)
recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

engine_obj_path := $(obj_path)/engine

cpp := $(call recurse,,*.cpp)
c   := $(call recurse,,*.c)


c_obj   := $(c:%=$(engine_obj_path)/%.o)
cpp_obj := $(cpp:%=$(engine_obj_path)/%.o)

engine_cpp_flags := $(preprocessor_flags) -DSM_EXPORT

engine_debug_linker_flags := -Wl,--pdb=$(subst $(dll_ext),.pdb,$(target))
engine_linker_flags       := $(linker_flags) $(if $(IS_DEBUG),$(engine_debug_linker_flags),)

all: print_info $(target)

print_info:
	@echo Make: $(liquid_engine_dll)

$(target): $(c_obj) $(cpp_obj)
	@echo "    Make: linking $(liquid_engine_dll) . . ."
	@$(CL) $^ -o $@ $(engine_cpp_flags) $(engine_linker_flags) -shared

$(engine_obj_path)/%.c.o:%.c
	@echo "    Make: compiling $< . . ."
	@mkdir -p $(dir $@)
	@$(CC) $< -o $@ $(compiler_flags) -c $(engine_cpp_flags) $(INCLUDE_PATHS)

$(engine_obj_path)/%.cpp.o:%.cpp
	@echo "    Make: compiling $< . . ."
	@mkdir -p $(dir $@)
	@$(CXX) $< -o $@ $(compiler_flags) -c $(engine_cpp_flags) $(INCLUDE_PATHS)

.PHONY: all print_info

-include $(deps)