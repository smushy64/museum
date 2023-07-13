# * Description:  Makefile for shaders
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: June 13, 2023

SLC := glslc -std=450core --target-env=opengl --target-spv=spv1.6 -x glsl
SL_DEBUG_C_FLAGS := -O0
SL_RELEASE_C_FLAGS := -O
SL_C_FLAGS := -I.

sl_flags := $(SL_C_FLAGS) $(if $(IS_DEBUG),$(SL_DEBUG_C_FLAGS),$(SL_RELEASE_C_FLAGS))

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

TARGET := ../$(SHADERS_PATH)

vert := $(call recurse,,*.vert)
frag := $(call recurse,,*.frag)

vert_spv := $(vert:%=$(TARGET)/%.spv)
frag_spv := $(frag:%=$(TARGET)/%.spv)

all: $(vert_spv) $(frag_spv)

spit:
	@echo $(vert_spv)

$(TARGET)/%.vert.spv:%.vert
	@echo "Make:    compiling $< . . ."
	@mkdir -p $(dir $@)
	@$(SLC) -c $< -o $@ $(sl_flags)

$(TARGET)/%.frag.spv:%.frag
	@echo "Make:    compiling $< . . ."
	@mkdir -p $(dir $@)
	@$(SLC) -c $< -o $@ $(sl_flags)

.PHONY: all spit
