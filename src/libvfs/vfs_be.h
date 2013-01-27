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
struct path;

struct vfsmount {
//        struct list_head mnt_hash;
        struct vfsmount *mnt_parent;    /* fs we are mounted on */
        struct dentry *mnt_mountpoint;  /* dentry of mountpoint */
        struct dentry *mnt_root;        /* root of the mounted tree */
        struct super_block *mnt_sb;     /* pointer to superblock */
#ifdef CONFIG_SMP
        struct mnt_pcp __percpu *mnt_pcp;
        atomic_t mnt_longterm;          /* how many of the refs are longterm */
#else
        int mnt_count;
        int mnt_writers;
#endif
//        struct list_head mnt_mounts;    /* list of children, anchored here */
//        struct list_head mnt_child;     /* and going through their mnt_child */
        int mnt_flags;
        /* 4 bytes hole on 64bits arches without fsnotify */
#ifdef CONFIG_FSNOTIFY
        __u32 mnt_fsnotify_mask;
        struct hlist_head mnt_fsnotify_marks;
#endif
        const char *mnt_devname;        /* Name of device e.g. /dev/dsk/hda1 */
//        struct list_head mnt_list;
//        struct list_head mnt_expire;    /* link in fs-specific expiry list */
//        struct list_head mnt_share;     /* circular list of shared mounts */
//        struct list_head mnt_slave_list;/* list of slave mounts */
//        struct list_head mnt_slave;     /* slave list entry */
        struct vfsmount *mnt_master;    /* slave is on master->mnt_slave_list */
        struct mnt_namespace *mnt_ns;   /* containing namespace */
        int mnt_id;                     /* mount identifier */
        int mnt_group_id;               /* peer group identifier */
        int mnt_expiry_mark;            /* true if marked for expiry */
        int mnt_pinned;
        int mnt_ghosts;
};

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

struct dentry {
        /* RCU lookup touched fields */
        unsigned int d_flags;           /* protected by d_lock */
//        seqcount_t d_seq;               /* per dentry seqlock */
//        struct hlist_bl_node d_hash;    /* lookup hash list */
        struct dentry *d_parent;        /* parent directory */
//        struct qstr d_name;
        struct inode *d_inode;          /* Where the name belongs to - NULL is
                                         * negative */
//        unsigned char d_iname[DNAME_INLINE_LEN];        /* small names */

        /* Ref lookup also touches following */
        unsigned int d_count;           /* protected by d_lock */
//        spinlock_t d_lock;              /* per dentry lock */
        const struct dentry_operations *d_op;
        struct super_block *d_sb;       /* The root of the dentry tree */
        unsigned long d_time;           /* used by d_revalidate */
        void *d_fsdata;                 /* fs-specific data */

//        struct list_head d_lru;         /* LRU list */
//        /*
//         * d_child and d_rcu can share memory
//         */
//        union {
//                struct list_head d_child;       /* child of parent list */
//                struct rcu_head d_rcu;
//        } d_u;
//        struct list_head d_subdirs;     /* our children */
//        struct list_head d_alias;       /* inode alias list */
};

extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);

extern struct dentry *mount_bdev(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data,
	int (*fill_super)(struct super_block *, void *, int));

#endif /* VFS_BE_H_ */
