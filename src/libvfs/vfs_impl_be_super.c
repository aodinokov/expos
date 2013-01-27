/*
 * vfs_impl_be_super.c
 *
 *  Created on: Jan 27, 2013
 *      Author: mralex
 */

#include "vfs_be.h"

struct dentry *
mount_fs(struct file_system_type *type, int flags, const char *name, void *data)
{
	struct dentry *root;
	struct super_block *sb;
	char *secdata = NULL;
	int error = -ENOMEM;

	if (data && !(type->fs_flags & FS_BINARY_MOUNTDATA)) {
		secdata = alloc_secdata();
		if (!secdata)
			goto out;

		error = security_sb_copy_data(data, secdata);
		if (error)
			goto out_free_secdata;
	}

	root = type->mount(type, flags, name, data);
	if (IS_ERR(root)) {
		error = PTR_ERR(root);
		goto out_free_secdata;
	}
	sb = root->d_sb;
	BUG_ON(!sb);
	WARN_ON(!sb->s_bdi);
	WARN_ON(sb->s_bdi == &default_backing_dev_info);
	sb->s_flags |= MS_BORN;

	error = security_sb_kern_mount(sb, flags, secdata);
	if (error)
		goto out_sb;

	/*
	 * filesystems should never set s_maxbytes larger than MAX_LFS_FILESIZE
	 * but s_maxbytes was an unsigned long long for many releases. Throw
	 * this warning for a little while to try and catch filesystems that
	 * violate this rule. This warning should be either removed or
	 * converted to a BUG() in 2.6.34.
	 */
	WARN((sb->s_maxbytes < 0), "%s set sb->s_maxbytes to "
		"negative value (%lld)\n", type->name, sb->s_maxbytes);

	up_write(&sb->s_umount);
	free_secdata(secdata);
	return root;
out_sb:
	dput(root);
	deactivate_locked_super(sb);
out_free_secdata:
	free_secdata(secdata);
out:
	return ERR_PTR(error);
}


struct dentry *mount_bdev(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data,
	int (*fill_super)(struct super_block *, void *, int))
{
//	struct block_device *bdev;
//	struct super_block *s;
//	fmode_t mode = FMODE_READ | FMODE_EXCL;
//	int error = 0;
//
//	if (!(flags & MS_RDONLY))
//		mode |= FMODE_WRITE;
//
//	bdev = blkdev_get_by_path(dev_name, mode, fs_type);
//	if (IS_ERR(bdev))
//		return ERR_CAST(bdev);
//
//	/*
//	 * once the super is inserted into the list by sget, s_umount
//	 * will protect the lockfs code from trying to start a snapshot
//	 * while we are mounting
//	 */
//	mutex_lock(&bdev->bd_fsfreeze_mutex);
//	if (bdev->bd_fsfreeze_count > 0) {
//		mutex_unlock(&bdev->bd_fsfreeze_mutex);
//		error = -EBUSY;
//		goto error_bdev;
//	}
//	s = sget(fs_type, test_bdev_super, set_bdev_super, bdev);
//	mutex_unlock(&bdev->bd_fsfreeze_mutex);
//	if (IS_ERR(s))
//		goto error_s;
//
//	if (s->s_root) {
//		if ((flags ^ s->s_flags) & MS_RDONLY) {
//			deactivate_locked_super(s);
//			error = -EBUSY;
//			goto error_bdev;
//		}
//
//		/*
//		 * s_umount nests inside bd_mutex during
//		 * __invalidate_device().  blkdev_put() acquires
//		 * bd_mutex and can't be called under s_umount.  Drop
//		 * s_umount temporarily.  This is safe as we're
//		 * holding an active reference.
//		 */
//		up_write(&s->s_umount);
//		blkdev_put(bdev, mode);
//		down_write(&s->s_umount);
//	} else {
//		char b[BDEVNAME_SIZE];
//
//		s->s_flags = flags;
//		s->s_mode = mode;
//		strlcpy(s->s_id, bdevname(bdev, b), sizeof(s->s_id));
//		sb_set_blocksize(s, block_size(bdev));
//		error = fill_super(s, data, flags & MS_SILENT ? 1 : 0);
//		if (error) {
//			deactivate_locked_super(s);
//			goto error;
//		}
//
//		s->s_flags |= MS_ACTIVE;
//		bdev->bd_super = s;
//	}
//
//	return dget(s->s_root);
//
//error_s:
//	error = PTR_ERR(s);
//error_bdev:
//	blkdev_put(bdev, mode);
//error:
//	return ERR_PTR(error);
	return 0;
}

