include pre.mk

MODULE:=esyslog
$(MODULE)_SOURCES_DIR:=src/liblog/esyslog
# common sources (use find -name '*.c' |sed 's/^.\///;s/$/ \\/' to update)
$(MODULE)_SOURCES:= \
		libesyslog_impl.c \
		ctrl/esyslogctrl_server.c \
		ctrl/libesyslogctrl_stub.c \
		ctrl/esyslogctrl_client.c \
		ctrl/libesyslogctrl_proxy.c \
		ctrl/libesyslogctrl_ep.c \
		backend/libesyslog2syslog_ep.c \
		backend/libesyslog2file_ep.c \
		backend/libesyslog2stderr_ep.c \
		backend/libesyslog2stderr.c \
		backend/libesyslog2syslog.c \
		backend/libesyslog2file.c \

# common part
$(MODULE)_BUILD_DIR:=$(BUILD_DIR)/liblog/$(MODULE)
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

# compile flags
$($(MODULE)_TARGET_BUILD_DIR)/%: CFLAGS=-I. -Isrc/liblog $(BUILD_CFLAGS)
$($(MODULE)_TARGET_BUILD_DIR)/backend/%: CFLAGS=-D_GNU_SOURCE -I. -Isrc/liblog/esyslog -Isrc/liblog/esyslog/backend $(BUILD_CFLAGS)
$($(MODULE)_TARGET_BUILD_DIR)/ctrl/%: CFLAGS=-D_GNU_SOURCE -I. -Isrc/liblog/esyslog -Isrc/liblog/esyslog/backend $(BUILD_CFLAGS)
$($(MODULE)_HOST_BUILD_DIR)/%: CFLAGS=-I. -Isrc/liblog $(BUILD_CFLAGS)
$($(MODULE)_HOST_BUILD_DIR)/backend/%: CFLAGS=-D_GNU_SOURCE -I. -Isrc/liblog/esyslog -Isrc/liblog/esyslog/backend $(BUILD_CFLAGS)
$($(MODULE)_HOST_BUILD_DIR)/ctrl/%: CFLAGS=-D_GNU_SOURCE -I. -Isrc/liblog/esyslog -Isrc/liblog/esyslog/backend $(BUILD_CFLAGS)

# target config
$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) #-Wl,-soname,libesyslog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so.$(BUILD_VER),\
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog_impl.o)
$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so: $($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2syslog.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) #-Wl,-soname,libesyslog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2syslog.so.$(BUILD_VER),\
				$($(MODULE)_TARGET_BUILD_DIR)/backend/libesyslog2syslog.o \
				$($(MODULE)_TARGET_BUILD_DIR)/backend/libesyslog2syslog_ep.o)
$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2syslog.so: $($(MODULE)_TARGET_BUILD_DIR)/libesyslog2syslog.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2file.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) #-Wl,-soname,libesyslog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2file.so.$(BUILD_VER),\
				$($(MODULE)_TARGET_BUILD_DIR)/backend/libesyslog2file.o \
				$($(MODULE)_TARGET_BUILD_DIR)/backend/libesyslog2file_ep.o)
$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2file.so: $($(MODULE)_TARGET_BUILD_DIR)/libesyslog2file.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2stderr.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) #-Wl,-soname,libesyslog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2stderr.so.$(BUILD_VER),\
				$($(MODULE)_TARGET_BUILD_DIR)/backend/libesyslog2stderr.o \
				$($(MODULE)_TARGET_BUILD_DIR)/backend/libesyslog2stderr_ep.o)
$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2stderr.so: $($(MODULE)_TARGET_BUILD_DIR)/libesyslog2stderr.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

$($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so.$(BUILD_VER):LDFLAGS:=-shared -pthread $(BUILD_LDFLAGS) #-Wl,-soname,libesyslog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so.$(BUILD_VER),\
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so \
				$($(MODULE)_TARGET_BUILD_DIR)/ctrl/libesyslogctrl_stub.o \
				$($(MODULE)_TARGET_BUILD_DIR)/ctrl/libesyslogctrl_proxy.o \
				$($(MODULE)_TARGET_BUILD_DIR)/ctrl/libesyslogctrl_ep.o)
$($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so: $($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

$($(MODULE)_TARGET_BUILD_DIR)/esyslogctrld:LDFLAGS:=-pthread $(BUILD_LDFLAGS)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/esyslogctrld,\
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so \
				$($(MODULE)_TARGET_BUILD_DIR)/ctrl/esyslogctrl_server.o)

$($(MODULE)_TARGET_BUILD_DIR)/esyslogctrl:LDFLAGS:= $(BUILD_LDFLAGS)
$(call ld_target, \
				$($(MODULE)_TARGET_BUILD_DIR)/esyslogctrl,\
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so \
				$($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so \
				$($(MODULE)_TARGET_BUILD_DIR)/ctrl/esyslogctrl_client.o)


# host config
$($(MODULE)_HOST_BUILD_DIR)/libesyslog.so.$(BUILD_VER):LDFLAGS:=-shared $(BUILD_LDFLAGS) #-Wl,-soname,libesyslog.so.$(BUILD_VER)
$(call ld_target, \
				$($(MODULE)_HOST_BUILD_DIR)/libesyslog.so.$(BUILD_VER),\
				$($(MODULE)_HOST_BUILD_DIR)/libesyslog_impl.o)
$($(MODULE)_HOST_BUILD_DIR)/libesyslog.so: $($(MODULE)_HOST_BUILD_DIR)/libesyslog.so.$(BUILD_VER)
		@cp -asuf `pwd`/$^ `pwd`/$@

## unittest sources
#$(MODULE)_UT_SOURCES:= \
#		unittest/ts1.c \
#		unittest/main.c \
#
#$(MODULE)_HOST_UT_OBJECTS:=$($(MODULE)_UT_SOURCES:%.c=$($(MODULE)_HOST_BUILD_DIR)/%.o)
#$($(MODULE)_HOST_UT_OBJECTS):$(BUILD_DIR)/check/install/.dir
#
#$($(MODULE)_HOST_BUILD_DIR)/unittest/%: CFLAGS=-I. -Isrc -I$(BUILD_DIR)/check/install/usr/include -Isrc/check_ext $(BUILD_CFLAGS)
#$(call map_dst_src_deps.c,$($(MODULE)_HOST_BUILD_DIR)/unittest,$(MODULE)_SOURCES_DIR/unittest,)
#-include $($($(MODULE)_HOST_UT_OBJECTS):.o=.d)
#$(call ld_target,$($(MODULE)_HOST_BUILD_DIR)/test_libesyslog,\
#						$($(MODULE)_HOST_UT_OBJECTS) \
#						$($(MODULE)_HOST_BUILD_DIR)/libesyslog.so \
#						$(BUILD_DIR)/check/install/usr/lib/libcheck.a)

#UNITTESTS+=$($(MODULE)_HOST_BUILD_DIR)/test_libesyslog
TARGETS+=$($(MODULE)_TARGET_BUILD_DIR)/libesyslog.so \
		$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2syslog.so \
		$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2file.so \
		$($(MODULE)_TARGET_BUILD_DIR)/libesyslog2stderr.so \
		$($(MODULE)_TARGET_BUILD_DIR)/libesyslogctrl.so \
		$($(MODULE)_TARGET_BUILD_DIR)/esyslogctrld \
		$($(MODULE)_TARGET_BUILD_DIR)/esyslogctrl \

include post.mk
