#include "check_ext.h"
#include "libvfs/vfs.h"
#include "libvfs/vfs_be.h"

static int ext3_fill_super (struct super_block *sb, void *data, int silent) {
	return 0;
}
static struct dentry *ext3_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data) {
	//return mount_bdev(fs_type, flags, dev_name, data, ext3_fill_super);
	return 0;
}

START_TEST(vfs_test_reg_unreg) {
	struct file_system_type ext3_fs_type = {
		.owner		= /*THIS_MODULE*/NULL,
		.name		= "ext3",
		.mount		= ext3_mount,
		.kill_sb	= NULL/*kill_block_super*/,
		.fs_flags	= 0/*FS_REQUIRES_DEV*/,
	};
	fail_unless(register_filesystem(&ext3_fs_type)==0, "register filesystem failed");
	fail_unless(unregister_filesystem(&ext3_fs_type)==0, "unregister filesystem failed");

//	int fd;
//	fd = vfs_open("/file", O_RDONLY, 0);
//	fail_unless(fd >= 0, "can't open file");
//	fd = vfs_close(fd);
//	fail_unless(fd == 0, "can't close file");
}END_TEST

START_TEST(vfs_test_open_close) {
	int fd;
	fd = vfs_open("/file", O_RDONLY, 0);
	fail_unless(fd >= 0, "can't open file");
	fd = vfs_close(fd);
	fail_unless(fd == 0, "can't close file");
}END_TEST

int ts1(void){
    return run_suite(
	START_SUITE(libvfs_basic){
		ADD_CASE(
			START_CASE(vfs_basic){
				ADD_TEST(vfs_test_reg_unreg);
				ADD_TEST(vfs_test_open_close);
			}END_CASE
		);
	}END_SUITE
    );
}
