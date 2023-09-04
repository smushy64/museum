# * Description:  Shaders build system
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: September 04, 2023

SCC    := glslc
SCCSTD := -std=450core --target-env=opengl -x glsl

SCCFLAGS :=

ifeq ($(RELEASE), true)
	SCCFLAGS += -O
else
	SCCFLAGS += -O0
endif

INCLUDE := -I.

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

BUILD_PATH := ../resources/shaders

VERT := $(call recurse,,*.vert)
FRAG := $(call recurse,,*.frag)

VERT_SPV := $(VERT:%=$(BUILD_PATH)/%.spv)
FRAG_SPV := $(FRAG:%=$(BUILD_PATH)/%.spv)

all: $(VERT_SPV) $(FRAG_SPV)

spit:
	@echo
	@echo "shader compiler: "$(SCC)
	@echo "shader standard: "$(SCCSTD)
	@echo "shader flags:    "$(SCCFLAGS)
	@echo "vert targets:    "$(VERT_SPV)
	@echo "frag targets:    "$(FRAG_SPV)

$(BUILD_PATH)/%.vert.spv:%.vert
	@echo "Make: compiling vertex shader $< . . ."
	@mkdir -p $(dir $@)
	@$(SCC) $(SCCSTD) -c $< -o $@ $(SCCFLAGS) -DGLSL_VERTEX=1

$(BUILD_PATH)/%.frag.spv:%.frag
	@echo "Make: compiling fragment shader $< . . ."
	@mkdir -p $(dir $@)
	@$(SCC) $(SCCSTD) -c $< -o $@ $(SCCFLAGS) -DGLSL_FRAGMENT=1

.PHONY: all spit

