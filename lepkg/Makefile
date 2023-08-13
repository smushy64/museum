# * Description:  Makefile for Liquid Engine Asset Package Utility
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: August 09, 2023

LEPKG_VERSION_MAJOR := 0
LEPKG_VERSION_MINOR := 1

CC := clang -std=c99

C_FLAGS := -O0 -g -Werror -Wall -Wextra -MMD -MP

ifeq ($(IS_WINDOWS), true)
	C_FLAGS += -gcodeview
endif

CPP_FLAGS := -DLEPKG_VERSION_MAJOR=$(LEPKG_VERSION_MAJOR)
CPP_FLAGS += -DLEPKG_VERSION_MINOR=$(LEPKG_VERSION_MINOR)
CPP_FLAGS += -DLEPKG_INTERNAL

LINK_FLAGS := -nostdlib++

ifeq ($(IS_WINDOWS), true)
	LINK_FLAGS += -fuse-ld=lld -Wl,//debug -static-libgcc -lmingw32
endif

INCLUDE_FLAGS := -I. -I../vendor

FLAGS := -MF ../$(object_path)/lepkg.d
FLAGS += $(C_FLAGS) $(CPP_FLAGS) $(INCLUDE_FLAGS) $(LINK_FLAGS)

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

c := $(call recurse,,*.c)
c := $(filter-out main.c,$(c))

corec := $(c)
corec := $(addsuffix \",$(corec))
corec := $(addprefix "#include \"",$(corec))

corec_path := corec.inl

deps := $(call recurse,../$(object_path),*d)

lepkg_path := ../$(BUILD_PATH)/lepkg

all: $(lepkg_path)

run: all
	@echo "Make: running lepkg . . ."
	@$(lepkg_path) -o ../test.lepkg --overwrite

$(lepkg_path): $(corec_path)
	@echo "Make: compiling lepkg . . ."
	@$(CC) main.c -o $(lepkg_path) $(FLAGS)

$(corec_path): $(c)
	@echo "// * Description:     Includes all c files" > $(corec_path)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(corec_path)
	@echo "// * File Generated:  "$(shell date) >> $(corec_path)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(corec_path)
	@echo "" >> $(corec_path)
	for i in $(corec); do echo $$i >> $(corec_path); done

.PHONY: all run

-include $(deps)

