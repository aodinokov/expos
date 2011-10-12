#!/usr/bin/make -f

ifneq ($(BUILD_VERBOSE),)
OLDSHELL := $(SHELL)
SHELL = $(info Building $@ $(if $?,(which depends on $?)))$(OLDSHELL)
endif

BUILD_TYPE=release
ifeq ($(BUILD_TYPE),debug)
BUILD_DIR=debug
BUILD_CFLAGS=-g -O0
else
ifeq ($(BUILD_TYPE),release)
BUILD_DIR=release
BUILD_CFLAGS=-O3
else
$(error Invalid build type)
endif
endif
BUILD_CFLAGS+=-DHAVE_CONFIG_H -fpic
BUILD_LDFLAG=-fpic
BUILD_VER=0.1

LD=$(CC)

#TODO: -MG  $1/%.d
define map_dst_src_deps_template.c
$1/%.o: $2/%.c $3
		@mkdir -p $$(@D)
		$$(CC) -c $$< $$(CFLAGS) -MMD -MF $1/$$*.d -MT '$1/$$*.o $1/$$*.d' -MP -o $1/$$*.o
endef
map_dst_src_deps.c = $(eval $(call map_dst_src_deps_template.c,$1,$2,$3))

define ld_target_template
$1: $2
		@mkdir -p $$(@D)
		$$(LD) 	$$(LDFLAGS) \
			$$(filter %.o,$$^) \
			$$(filter %.a,$$^) \
			$$(addprefix -L,$$(sort $$(dir $$(filter %.so,$$^)))) \
			$$(patsubst lib%.so,-l%,$$(notdir $$(filter %.so,$$^))) \
			-o $$@
endef
ld_target = $(eval $(call ld_target_template,$1,$2))

define extra_deps_template
$1: $2
endef
extra_deps=$(foreach file,$1,$(eval $(call extra_deps_template,$(file),$2)))

default: all

FORCE:

include src/check/module.mk
include src/check_ext/module.mk

targets: $(TARGETS)

unittests: FORCE $(UNITTESTS)
	#for i in $(UNITTESTS); do ./$$i; done
	-for i in $(UNITTESTS); do ./$$i; done

all: targets unittests

clean:
		rm -rf $(BUILD_DIR)/*

.PHONY: clean all FORCE targets unittests
