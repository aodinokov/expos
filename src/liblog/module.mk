include pre.mk

MODULE:=liblog
$(MODULE)_SOURCES_DIR:=src/liblog
# common sources (use find -name '*.c' |sed 's/^.\///;s/$/ \\/' to update)
$(MODULE)_SOURCES:= \

#	vfs_impl_open.c \
#	vfs_impl_close.c \
#	vfs_impl_be.c \

# common part
$(MODULE)_BUILD_DIR:=$(BUILD_DIR)/$(MODULE)
$(MODULE)_TARGET_BUILD_DIR:=$($(MODULE)_BUILD_DIR)/target
$(MODULE)_HOST_BUILD_DIR:=$($(MODULE)_BUILD_DIR)/host

# map $(MODULE)_SOURCES_DIR/*.c to $(MODULE)_HOST_BUILD_DIR/*.o and include dependencies
$(MODULE)_HOST_OBJECTS:=$($(MODULE)_SOURCES:%.c=$($(MODULE)_HOST_BUILD_DIR)/%.o)
$(call map_dst_src_deps.c,$($(MODULE)_HOST_BUILD_DIR),$($(MODULE)_SOURCES_DIR),)
-include $($($(MODULE)_HOST_OBJECTS):.o=.d)

# map $(MODULE)_SOURCES_DIR/*.c to $(MODULE)_TARGET_BUILD_DIR/*.o and include dependencies
$(MODULE)_TARGET_OBJECTS:=$($(MODULE)_SOURCES:%.c=$($(MODULE)_TARGET_BUILD_DIR)/%.o)
$(call map_dst_src_deps.c,$($(MODULE)_TARGET_BUILD_DIR),$($(MODULE)_SOURCES_DIR),)
-include $($($(MODULE)_TARGET_OBJECTS):.o=.d)

# target compiler&linker
$($(MODULE)_TARGET_BUILD_DIR)/%: CC=$(TARGET_CC)
$($(MODULE)_TARGET_BUILD_DIR)/%: LD=$(TARGET_LD)

# target config
$($(MODULE)_TARGET_BUILD_DIR)/%: CFLAGS= -I. -Isrc/liblog -Isrc/liblog/include2  $(BUILD_CFLAGS)
$($(MODULE)_TARGET_BUILD_DIR)/liblog.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) -Wl,-soname,liblog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/liblog.so.$(BUILD_VER),\
				$($(MODULE)_TARGET_OBJECTS))
$($(MODULE)_TARGET_BUILD_DIR)/liblog.so: $($(MODULE)_TARGET_BUILD_DIR)/liblog.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

# host config
$($(MODULE)_HOST_BUILD_DIR)/%: CFLAGS=-I. -Isrc/liblog $(BUILD_CFLAGS)
$($(MODULE)_HOST_BUILD_DIR)/liblog.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) -Wl,-soname,liblog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_HOST_BUILD_DIR)/liblog.so.$(BUILD_VER),\
				$($(MODULE)_HOST_OBJECTS))
$($(MODULE)_HOST_BUILD_DIR)/liblog.so: $($(MODULE)_HOST_BUILD_DIR)/liblog.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

# unittest sources
$(MODULE)_UT_SOURCES:= \
		unittest/ts1.c \
		unittest/main.c \

$(MODULE)_HOST_UT_OBJECTS:=$($(MODULE)_UT_SOURCES:%.c=$($(MODULE)_HOST_BUILD_DIR)/%.o)
$($(MODULE)_HOST_UT_OBJECTS):$(BUILD_DIR)/check/install/.dir

$($(MODULE)_HOST_BUILD_DIR)/unittest/%: CFLAGS=-I. -Isrc -I$(BUILD_DIR)/check/install/usr/include -Isrc/check_ext $(BUILD_CFLAGS)
$(call map_dst_src_deps.c,$($(MODULE)_HOST_BUILD_DIR)/unittest,$(MODULE)_SOURCES_DIR/unittest,)
-include $($($(MODULE)_HOST_UT_OBJECTS):.o=.d)
$(call ld_target,$($(MODULE)_HOST_BUILD_DIR)/test_liblog,\
						$($(MODULE)_HOST_UT_OBJECTS) \
						$($(MODULE)_HOST_BUILD_DIR)/liblog.so \
						$(BUILD_DIR)/check/install/usr/lib/libcheck.a)

UNITTESTS+=$($(MODULE)_HOST_BUILD_DIR)/test_liblog
TARGETS+=$($(MODULE)_TARGET_BUILD_DIR)/liblog.so

include post.mk
