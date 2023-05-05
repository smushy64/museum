# * Description:  Makefile for Test Bed EXE
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023
# * Notes:        

MAKEFLAGS += -j4

target  := ../$(BUILD_PATH)/$(testbed_name)
recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

test_bed_obj_path := $(obj_path)/test_bed

cpp := $(call recurse,,*.cpp)
c   := $(call recurse,,*.c)

c_obj   := $(c:%=$(test_bed_obj_path)/%.o)
cpp_obj := $(cpp:%=$(test_bed_obj_path)/%.o)
pch     := $(test_bed_obj_path)/testbed_pch.gch

deps := $(call recurse,$(obj_path),*.d)

test_bed_cpp_flags := $(preprocessor_flags)

test_bed_debug_linker_flags := -Wl,--pdb=$(subst $(exe_ext),.pdb,$(target))
test_bed_linker_flags := $(linker_flags) $(if $(IS_DEBUG),$(test_bed_debug_linker_flags),)

test_bed_win_res := ../$(win_res)

all: print_info $(target)

print_info:
	@echo Make: $(testbed_name)

$(target): $(pch) $(c_obj) $(cpp_obj) $(if $(win_res), $(test_bed_win_res), )
	@echo "    Make: linking $(testbed_name) . . ."
	@$(CL) ../$(BUILD_PATH)/$(liquid_engine_dll) $^ -o $@ $(preprocessor_flags) $(test_bed_linker_flags)

$(test_bed_obj_path)/%.c.o:%.c
	@echo "    Make: compiling $< . . ."
	@mkdir -p $(dir $@)
	@$(CC) $< -o $@ $(compiler_flags) -c $(preprocessor_flags) $(INCLUDE_PATHS)

$(test_bed_obj_path)/%.cpp.o:%.cpp
	@echo "    Make: compiling $< . . ."
	@mkdir -p $(dir $@)
	@$(CXX) $< -o $@ $(compiler_flags) -c $(preprocessor_flags) $(INCLUDE_PATHS)

$(pch):pch.h
	@echo "    Make: compiling pre-compiled header . . ."
	@mkdir -p $(dir $@)
	@$(CXX) $(pch_flags) $< -o $@ $(compiler_flags) -c $(preprocessor_flags) $(INCLUDE_PATHS)

$(test_bed_win_res):
	@echo "    Make: compiling windows resources . . ."
	@mkdir -p $(BUILD_PATH)/obj/
	@windres ../win32/resources.rc -o $(test_bed_win_res)

.PHONY: all print_info

-include $(deps)