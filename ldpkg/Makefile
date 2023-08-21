# * Description:  Makefile for Liquid Engine Asset Package Utility
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: August 09, 2023

LDPKG_VERSION_MAJOR := 0
LDPKG_VERSION_MINOR := 1

C_FLAGS := -O0 -g -Werror -Wall -Wextra -MMD -MP
C_FLAGS += -Wno-missing-braces -ffast-math
C_FLAGS += -Werror=vla

ifeq ($(TARGET_ARCH), x86_64)
	C_FLAGS += -masm=intel -march=native
endif
ifeq ($(TARGET_ARCH), arm)
	C_FLAGS += -target-arm64
endif
ifeq ($(TARGET_ARCH), wasm)
	C_FLAGS += -target-wasm64
endif

ifeq ($(IS_WINDOWS), true)
	C_FLAGS += -gcodeview
endif

CPP_FLAGS := -DLDPKG_VERSION_MAJOR=$(LDPKG_VERSION_MAJOR)
CPP_FLAGS += -DLDPKG_VERSION_MINOR=$(LDPKG_VERSION_MINOR)
CPP_FLAGS += -DLDPKG_INTERNAL -DLD_EXPORT -DLD_SIMD_WIDTH=4
CPP_FLAGS += -DLD_ASSERTIONS -DLD_LOGGING

LINK_FLAGS :=

ifeq ($(IS_WINDOWS), true)
	LINK_FLAGS += -fuse-ld=lld -Wl,//debug -static-libgcc -lmingw32
endif

INCLUDE_FLAGS := -I. -I../vendor -I../liquid_engine

FLAGS := $(C_FLAGS) $(CPP_FLAGS) $(INCLUDE_FLAGS)
FLAGS += -MF ../$(object_path)/ldpkg.d
FLAGS += $(LINK_FLAGS)

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

c := $(call recurse,,*.c)
c := $(filter-out main.c,$(c))

corec := $(c)
corec := $(addsuffix \",$(corec))
corec := $(addprefix "#include \"",$(corec))

corec_path := corec.inl

deps := $(call recurse,../$(object_path),*d)

ldpkg_path := ../$(BUILD_PATH)/ldpkg

all: $(ldpkg_path)

run: all
	@echo "Make: running ldpkg . . ."
	@$(ldpkg_path) -o ../test.ldpkg --help

$(ldpkg_path): $(corec_path)
	@echo "Make: compiling ldpkg . . ."
	@$(CC) main.c -o $(ldpkg_path) $(FLAGS)

$(corec_path): $(c)
	@echo "// * Description:     Includes all c files" > $(corec_path)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(corec_path)
	@echo "// * File Generated:  "$(shell date) >> $(corec_path)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(corec_path)
	@echo "" >> $(corec_path)
	for i in $(corec); do echo $$i >> $(corec_path); done

.PHONY: all run

-include $(deps)

