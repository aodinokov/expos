/*
 * vfs_be.h
 *
 *  Created on: Jan 27, 2013
 *      Author: mralex
 */

#ifndef VFS_BE_H_
#define VFS_BE_H_

#include "vfs.h"

struct module;
struct super_block;

struct file_system_type {
        const char *name;
        int fs_flags;
        struct dentry *(*mount) (struct file_system_type *, int,
                       	   	   	 const char *, void *);
        void (*kill_sb) (struct super_block *);
        struct module *owner;
        struct file_system_type * next;
//        struct list_head fs_supers;

//        struct lock_class_key s_lock_key;
//        struct lock_class_key s_umount_key;
//        struct lock_class_key s_vfs_rename_key;
//
//        struct lock_class_key i_lock_key;
//        struct lock_class_key i_mutex_key;
//        struct lock_class_key i_mutex_dir_key;
//        struct lock_class_key i_alloc_sem_key;
};

extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);

#endif /* VFS_BE_H_ */
