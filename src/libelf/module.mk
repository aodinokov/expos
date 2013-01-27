MODULE:=libelf
include pre.mk
# use to update this list find -name '*.c' |sed 's/^.\///;s/$/ \\/'
$(MODULE)_sourses:= \
		verdef_32_tof.c \
		getbase.c \
		cntl.c \
		64.xlatetof.c \
		gelfphdr.c \
		data.c \
		gelfshdr.c \
		getident.c \
		32.fsize.c \
		getarsym.c \
		newscn.c \
		flag.c \
		x.remscn.c \
		rawdata.c \
		errmsg.c \
		hash.c \
		assert.c \
		32.getphdr.c \
		x.elfext.c \
		version.c \
		begin.c \
		newdata.c \
		gelftrans.c \
		32.getshdr.c \
		update.c \
		verdef_32_tom.c \
		rand.c \
		getdata.c \
		32.newehdr.c \
		getaroff.c \
		errno.c \
		x.movscn.c \
		input.c \
		swap64.c \
		memset.c \
		kind.c \
		ndxscn.c \
		next.c \
		nlist.c \
		32.newphdr.c \
		strptr.c \
		checksum.c \
		opt.delscn.c \
		gelfehdr.c \
		verdef_64_tom.c \
		cook.c \
		getscn.c \
		getarhdr.c \
		32.xlatetof.c \
		verdef_64_tof.c \
		end.c \
		rawfile.c \
		32.getehdr.c \
		nextscn.c \
		fill.c \

$(MODULE)_sourses_unittest:= \
		unittest/ts1.c \
		unittest/main.c \

#Target lib
$(MODULE)_objects_target:=$($(MODULE)_sourses:%.c=$(BUILD_DIR)/libelf/target/%.o)
$(BUILD_DIR)/libelf/target/%: CFLAGS=-I. -Isrc/libelf $(BUILD_CFLAGS)
$(BUILD_DIR)/libelf/target/%: CC=$(TARGET_CC)
$(BUILD_DIR)/libelf/target/%: LD=$(TARGET_LD)
$(call map_dst_src_deps.c,$(BUILD_DIR)/libelf/target,src/libelf,)
-include $($(MODULE)_objects_target:.o=.d)
$(BUILD_DIR)/libelf/target/libelf.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) -Wl,-soname,libelf.so.$(BUILD_VER)
$(call ld_target,$(BUILD_DIR)/libelf/target/libelf.so.$(BUILD_VER),\
				$($(MODULE)_objects_target))
$(BUILD_DIR)/libelf/target/libelf.so: $(BUILD_DIR)/libelf/target/libelf.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

#Lib for Unit-tests
$(MODULE)_objects_ut:=$($(MODULE)_sourses:%.c=$(BUILD_DIR)/libelf/ut/%.o)
$(BUILD_DIR)/libelf/ut/%: CFLAGS=-I. -Isrc/libelf $(BUILD_CFLAGS)
$(call map_dst_src_deps.c,$(BUILD_DIR)/libelf/ut,src/libelf,)
-include $($(MODULE)_objects_ut:.o=.d)
$(BUILD_DIR)/libelf/ut/libelf.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) -Wl,-soname,libelf.so.$(BUILD_VER)
$(call ld_target,$(BUILD_DIR)/libelf/ut/libelf.so.$(BUILD_VER),\
				$($(MODULE)_objects_ut))
$(BUILD_DIR)/libelf/ut/libelf.so: $(BUILD_DIR)/libelf/ut/libelf.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

#Unittests
$(MODULE)_objects_unittest:=$($(MODULE)_sourses_unittest:%.c=$(BUILD_DIR)/libelf/ut/%.o)
$(BUILD_DIR)/libelf/ut/unittest/%: CFLAGS=-I. -Isrc -I$(BUILD_DIR)/check/install/usr/include -Isrc/check_ext $(BUILD_CFLAGS)
$(call map_dst_src_deps.c,$(BUILD_DIR)/libelf/ut/unittest,src/libelf/unittest,)
-include $($(MODULE)_objects_unittest:.o=.d)
$(call ld_target,$(BUILD_DIR)/libelf/ut/test_libelf,\
						$($(MODULE)_objects_unittest) \
						$(BUILD_DIR)/libelf/ut/libelf.so \
						$(BUILD_DIR)/check/install/usr/lib/libcheck.a)

UNITTESTS+=$(BUILD_DIR)/libelf/ut/test_libelf
TARGETS+=$(BUILD_DIR)/libelf/target/libelf.so

include post.mk
