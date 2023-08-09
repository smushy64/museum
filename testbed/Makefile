# * Description:  Makefile for Test Bed EXE
# * Author:       Alicia Amarilla (smushyaa@gmail.com)
# * File Created: April 27, 2023
# * Notes:        

TESTBED_PATH := ../$(BUILD_PATH)/$(TESTBED_NAME)
TESTBED_COMPILE_FILE := entry.cpp

testbed_rc_flags :=
testbed_dc_flags := 
testbed_c_flags := $(c_flags) $(if $(IS_DEBUG),$(testbed_dc_flags),$(testbed_rc_flags))

ifeq ($(IS_LINUX), true)
	testbed_c_flags += -fPIC
endif

testbed_rcpp_flags :=
testbed_dcpp_flags := 
testbed_cpp_flags := $(cpp_flags) $(if $(IS_DEBUG),$(testbed_dcpp_flags),$(testbed_rcpp_flags))

testbed_rlink_flags := $(rlink_flags)
testbed_dlink_flags := $(dlink_flags)
testbed_link_flags := -fuse-ld=lld $(if $(IS_DEBUG),$(testbed_dlink_flags),$(testbed_rlink_flags))
testbed_link_flags += -nostdlib++ -shared

ifeq ($(IS_WINDOWS), true)
	testbed_link_flags += -lmingw32 -static-libgcc
	testbed_link_flags += ../$(EXE_PATH)
else
	testbed_link_flags += -Wl,-rpath,../$(BUILD_PATH)/$(EXE_NAME).a
endif

TESTBED_FLAGS := $(testbed_c_flags) $(testbed_cpp_flags) $(testbed_link_flags)
TESTBED_FLAGS += -I../liquid_engine -Itestbed
TESTBED_FLAGS += -MF ../$(object_path)/$(TESTBED_NAME).d

recurse = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recurse,$d/,$2))

deps := $(call recurse,../$(object_path),*d)

cpp := $(call recurse,,*.cpp)
cpp := $(filter-out $(TESTBED_COMPILE_FILE),$(cpp))
h   := $(call recurse,,*.h)

corecpp := $(cpp)
corecpp := $(addsuffix \",$(corecpp))
corecpp := $(addprefix "#include \"",$(corecpp))

corecpp_path := corecpp.inl

all: $(TESTBED_PATH)

$(TESTBED_PATH): $(corecpp_path)
	@echo "Make: compiling" $(TESTBED_NAME) ". . ."
	@mkdir -p ../$(object_path)
	@$(CC) $(TESTBED_COMPILE_FILE) -o $(TESTBED_PATH) $(TESTBED_FLAGS)

$(corecpp_path): $(cpp)
	@echo "// * Description:     Includes all cpp files" > $(corecpp_path)
	@echo "// * Author:          Alicia Amarilla (smushyaa@gmail.com)" >> $(corecpp_path)
	@echo "// * File Generated:  "$(shell date) >> $(corecpp_path)
	@echo "// IMPORTANT(alicia): This file should only ever be included ONCE." >> $(corecpp_path)
	@echo "" >> $(corecpp_path)
	for i in $(corecpp); do echo $$i >> $(corecpp_path); done

spit:
	@echo testbed spit:
	@echo $(TESTBED_FLAGS)

.PHONY: all spit

-include $(deps)

