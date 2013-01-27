MODULE:=check_ext
include pre.mk
# use to update this list find -name '*.c' |sed 's/^.\///;s/$/ \\/'
$(MODULE)_sourses:= \
						examples/example1.c \

$(MODULE)_objects:=$($(MODULE)_sourses:%.c=$(BUILD_DIR)/check_ext/%.o)
$($(MODULE)_objects):$(BUILD_DIR)/check/install/.dir
$(BUILD_DIR)/check_ext/%: CFLAGS=-Isrc/check_ext -I$(BUILD_DIR)/check/install/usr/include $(BUILD_CFLAGS)

$(call map_dst_src_deps.c,$(BUILD_DIR)/check_ext,src/check_ext,)

-include $($(MODULE)_objects:.o=.d)

$(call ld_target,$(BUILD_DIR)/check_ext/check_ext_example,\
						$($(MODULE)_objects) \
						$(BUILD_DIR)/check/install/usr/lib/libcheck.a)

#UNITTESTS+=$(BUILD_DIR)/check_ext/check_ext_example

include post.mk
