#!/usr/bin/make -f

ifneq ($(BUILD_VERBOSE),)
OLDSHELL := $(SHELL)
SHELL = $(info Building $@ $(if $?,(which depends on $?)))$(OLDSHELL)
endif

BUILD_TYPE:=debug
ifeq ($(BUILD_TYPE),debug)
BUILD_DIR:=debug-$(CC)
BUILD_CFLAGS:=-g -O0
else
ifeq ($(BUILD_TYPE),release)
BUILD_DIR:=release-$(CC)
BUILD_CFLAGS:=-O3
else
$(error Invalid build type)
endif
endif
BUILD_CFLAGS+=-DHAVE_CONFIG_H -fpic
BUILD_LDFLAG:=-fpic
BUILD_VER:=0.1

ifeq ($(TARGET_CC),)
TARGET_CC:=$(CC)
endif
LD:=$(CC)
TARGET_LD:=$(TARGET_CC)

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

# to make these variables immidiate
TARGETS:=
UNITTESTS:=

include src/check/module.mk
include src/check_ext/module.mk
include src/liblog/module.mk
include src/libvfs/module.mk
include src/libelf/module.mk


targets: $(TARGETS)

unittests: FORCE $(UNITTESTS)
	@res=0; \
	for i in $(UNITTESTS); do \
		(cd `dirname $$i`; LD_LIBRARY_PATH=`pwd` ./`basename $$i`); \
		tmp=$$?; if [ $$tmp -ne 0 ]; then res=$$tmp; fi; \
	done; \
	if [ $$res -ne 0 ]; then echo "One or more unit-tests finished with an error. Build failed." >&2; exit $$res; fi

# it's temporary solution - better to allow modules to decide which paths/files is necessary to add as soursedir
# TODO: compare current filelist with previous and modify cscope.files in that case
#previous approach: SOURCEDIRS=`find ./src -type d | sed ':a;N;$$!ba;s/\n/:/g'` cscope </dev/null 2>&1 1>/dev/null
CSCOPE_DIR=cscope
$(CSCOPE_DIR)/cscope.files: FORCE
	@mkdir -p $(@D)
	find `pwd`/src -name '*.[ch]*' >$@

$(CSCOPE_DIR)/cscope.out: $(CSCOPE_DIR)/cscope.files
	cd $(^D) && cscope -b -q

cscope: $(CSCOPE_DIR)/cscope.out

all: targets cscope unittests

clean:
		rm -rf $(BUILD_DIR)/*

.PHONY: clean all FORCE targets unittests cscope
