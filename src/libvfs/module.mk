MODULE:=libvfs
include pre.mk
# use to update this list find -name '*.c' |sed 's/^.\///;s/$/ \\/'

$(MODULE)_sourses:= \
	vfs_impl_open.c \
	vfs_impl_close.c \
	vfs_impl_be.c \


$(MODULE)_sourses_unittest:= \
		unittest/ts1.c \
		unittest/main.c \

#Target lib -nostdinc -Isrc/libvfs/include	
$(MODULE)_objects_target:=$($(MODULE)_sourses:%.c=$(BUILD_DIR)/libvfs/target/%.o)
$(BUILD_DIR)/libvfs/target/%: CFLAGS= -I. -Isrc/libvfs -Isrc/libvfs/include2  $(BUILD_CFLAGS)
$(BUILD_DIR)/libvfs/target/%: CC=$(TARGET_CC)
$(BUILD_DIR)/libvfs/target/%: LD=$(TARGET_LD)
$(call map_dst_src_deps.c,$(BUILD_DIR)/libvfs/target,src/libvfs,)
-include $($(MODULE)_objects_target:.o=.d)
$(BUILD_DIR)/libvfs/target/libvfs.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) -Wl,-soname,libvfs.so.$(BUILD_VER)
$(call ld_target,$(BUILD_DIR)/libvfs/target/libvfs.so.$(BUILD_VER),\
				$($(MODULE)_objects_target))
$(BUILD_DIR)/libvfs/target/libvfs.so: $(BUILD_DIR)/libvfs/target/libvfs.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

#Lib for Unit-tests
$(MODULE)_objects_ut:=$($(MODULE)_sourses:%.c=$(BUILD_DIR)/libvfs/ut/%.o)
$(BUILD_DIR)/libvfs/ut/%: CFLAGS=-I. -Isrc/libvfs $(BUILD_CFLAGS)
$(call map_dst_src_deps.c,$(BUILD_DIR)/libvfs/ut,src/libvfs,)
-include $($(MODULE)_objects_ut:.o=.d)
$(BUILD_DIR)/libvfs/ut/libvfs.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) -Wl,-soname,libvfs.so.$(BUILD_VER)
$(call ld_target,$(BUILD_DIR)/libvfs/ut/libvfs.so.$(BUILD_VER),\
				$($(MODULE)_objects_ut))
$(BUILD_DIR)/libvfs/ut/libvfs.so: $(BUILD_DIR)/libvfs/ut/libvfs.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

#Unittests
$(MODULE)_objects_unittest:=$($(MODULE)_sourses_unittest:%.c=$(BUILD_DIR)/libvfs/ut/%.o)
$(BUILD_DIR)/libvfs/ut/unittest/%: CFLAGS=-I. -Isrc -I$(BUILD_DIR)/check/install/usr/include -Isrc/check_ext $(BUILD_CFLAGS)
$($(MODULE)_objects_unittest):$(BUILD_DIR)/check/install/.dir
$(call map_dst_src_deps.c,$(BUILD_DIR)/libelf/ut/unittest,src/libelf/unittest,)
-include $($(MODULE)_objects_unittest:.o=.d)
$(call ld_target,$(BUILD_DIR)/libvfs/ut/test_libvfs,\
						$($(MODULE)_objects_unittest) \
						$(BUILD_DIR)/libvfs/ut/libvfs.so \
						$(BUILD_DIR)/check/install/usr/lib/libcheck.a)

UNITTESTS+=$(BUILD_DIR)/libvfs/ut/test_libvfs
#TARGETS+=$(BUILD_DIR)/libvfs/target/libvfs.so

include post.mk
