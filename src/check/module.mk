# The module name
#
MODULE:=check
include pre.mk

CHECK_NAME:=check-0.9.8
CHECK_ARCHIVE:=$(CHECK_NAME).tar.gz
CHECK_PATCHES:=

CHECK_SRC:=src/check

# calc work diffs
ifneq ($(CHECK_PATCH_EXCLUDE),)
ifeq ($(CHECK_PATCH_EXCLUDE),all)
CHECK_PATCHES:=
else
CHECK_PATCHES:=$(filter-out $(CHECK_PATCH_EXCLUDE),$(CHECK_PATCHES))
endif
endif

$(info Module '$(MODULE)' patches: $(CHECK_PATCHES))

# convert diffs to diffspath
CHECK_PATCH_PATHS:=$(addprefix  $(CHECK_SRC)/, $(CHECK_PATCHES))

$(BUILD_DIR)/check/%: CHECK_CONFIGURE:=./configure -q --disable-dependency-tracking --prefix=/usr

$(BUILD_DIR)/check/install/%: $(BUILD_DIR)/check/install/.dir

# workaroung with touch: build directory is present - that means that somebody making changes and we want to rebuild it every time
$(BUILD_DIR)/check/install/.dir: $(BUILD_DIR)/check/build/Makefile
	-rm -rf ${@D}
	mkdir -p ${@D}
	$(MAKE) -j 1 -C $(dir $<) all install DESTDIR=`pwd`/${@D}
	[ -d $(dir ${@D})work ] || touch $@ 

$(BUILD_DIR)/check/build/Makefile: $(BUILD_DIR)/check/build/.dir
	cd ${@D} && \
	    $(CHECK_CONFIGURE)

# workaround with .tmp: --strip-components=1 isn't supported by current tar from env
$(BUILD_DIR)/check/%/.dir:	$(CHECK_SRC)/$(CHECK_ARCHIVE) \
				$(CHECK_PATCH_PATHS)
	@-rm -rf ${@D}
	@-rm -rf ${@D}.tmp
	@mkdir -p ${@D}
	@mkdir -p ${@D}.tmp
	@tar -xzf $< -C ${@D}.tmp
	@mv ${@D}.tmp/$(CHECK_NAME)/* ${@D}
	@-rm -rf ${@D}.tmp
	@cd ${@D} && for x in $$(echo "$(filter-out $<, $^)"); do patch -p1 < $$x 1>/dev/null; done
	@touch $@

$(BUILD_DIR)/check/workspace: $(BUILD_DIR)/check/work/.dir
	-rm -rf ${@D}/build
	-rm -rf ${@D}/install
	@cp -asuf `pwd`/${@D}/work `pwd`/${@D}/build
	cd ${@D}/build && \
	    $(CHECK_CONFIGURE)

$(BUILD_DIR)/check/diff: \
			$(BUILD_DIR)/check/work/.dir \
			$(BUILD_DIR)/check/pristine/.dir
	@cd ${@D} && diff --show-c-function -N -r pristine work; \
	    res=$$?; \
	    rm -rf pristine; \
	    if [ $$res -eq 2 ]; then echo "diff faces some troubles"; exit $$res; fi; \
	    exit 0;

.PHONY: $(BUILD_DIR)/check/workspace $(BUILD_DIR)/check/diff

#TARGETS+=$(BUILD_DIR)/check/install/.dir
$(BUILD_DIR)/check/install/usr/lib/libcheck.so: $(BUILD_DIR)/check/install/.dir


include post.mk
