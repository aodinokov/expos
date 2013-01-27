/*
 * vfs.h
 *
 *  Created on: Jan 27, 2013
 *      Author: mralex
 */

#ifndef VFS_H_
#define VFS_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>

// Documentation/filesystems/path-lookup.txt
//TODO: mode_t, struct stat
int 		vfs_mount(const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data);
int 		vfs_umount(const char *target);
int 		vfs_umount2(const char *target, int flags);

int 		vfs_open(const char *pathname, int flags, mode_t mode);
int 		vfs_creat(const char *pathname, mode_t mode);

ssize_t 	vfs_read(int fd, void *buf, size_t count);
ssize_t 	vfs_write(int fd, const void *buf, size_t count);
int 		vfs_fstat(int fd, struct stat *buf);
int 		vfs_fchmod(int fd, mode_t mode);
int 		vfs_close(int fd);

int 		vfs_symlink(const char *oldpath, const char *newpath);
int 		vfs_link(const char *oldpath, const char *newpath);
int 		vfs_unlink(const char *pathname);

int 		vfs_stat(const char *path, struct stat *buf);
int 		vfs_lstat(const char *path, struct stat *buf);
int 		vfs_chmod(const char *path, mode_t mode);

#endif /* VFS_H_ */
