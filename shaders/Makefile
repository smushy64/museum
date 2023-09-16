# * Description:  Shaders build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

BUILD_PATH := ../resources/shaders
SCC    := glslc
SCCSTD := -std=450core --target-env=opengl -x glsl

SCCFLAGS := -MD

ifeq ($(RELEASE), true)
	SCCFLAGS += -O
else
	SCCFLAGS += -O0
endif

INCLUDE := -I.

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

VERT := $(call recurse,,*.vert)
FRAG := $(call recurse,,*.frag)
GEOM := $(call recurse,,*.geom)
COMP := $(call recurse,,*.comp)

VERT_SPV := $(VERT:%=$(BUILD_PATH)/%.spv)
FRAG_SPV := $(FRAG:%=$(BUILD_PATH)/%.spv)
GEOM_SPV := $(GEOM:%=$(BUILD_PATH)/%.spv)
COMP_SPV := $(COMP:%=$(BUILD_PATH)/%.spv)

dependencies := $(call recurse,$(BUILD_PATH),*.d)

all: $(VERT_SPV) $(FRAG_SPV) $(GEOM_SPV) $(COMP_SPV)

spit:
	@echo
	@echo "shader compiler: "$(SCC)
	@echo "shader standard: "$(SCCSTD)
	@echo "shader flags:    "$(SCCFLAGS)
	@echo "vert targets:    "$(VERT_SPV)
	@echo "frag targets:    "$(FRAG_SPV)
	@echo "dependencies:    "$(dependencies)

$(BUILD_PATH)/%.vert.spv:%.vert
	@echo "Make: compiling vertex shader $< . . ."
	@mkdir -p $(dir $@)
	@$(SCC) $(SCCSTD) -c $< -o $@ $(SCCFLAGS) -DGLSL_VERTEX=1

$(BUILD_PATH)/%.frag.spv:%.frag
	@echo "Make: compiling fragment shader $< . . ."
	@mkdir -p $(dir $@)
	@$(SCC) $(SCCSTD) -c $< -o $@ $(SCCFLAGS) -DGLSL_FRAGMENT=1

$(BUILD_PATH)/%.geom.spv:%.geom
	@echo "Make: compiling geometry shader $< . . ."
	@mkdir -p $(dir $@)
	@$(SCC) $(SCCSTD) -c $< -o $@ $(SCCFLAGS) -DGLSL_GEOMETRY=1

$(BUILD_PATH)/%.comp.spv:%.comp
	@echo "Make: compiling compute shader $< . . ."
	@mkdir -p $(dir $@)
	@$(SCC) $(SCCSTD) -c $< -o $@ $(SCCFLAGS) -DGLSL_COMPUTE=1


.PHONY: all spit

-include $(dependencies)

