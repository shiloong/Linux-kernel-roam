```
start_kernel
|-> vfs_caches_init_early
    |-> dcache_init_early
    |-> inode_init_early

...

|-> vfs_caches_init
    |-> dcache_init();
    |-> inode_init();
    |-> files_init();
    |-> files_maxfiles_init();
    |-> mnt_init();
        |-> kernfs_init
        |-> sysfs_init
            |-> kernfs_create_root
            |-> register_filesystem
        |-> init_rootfs
            |-> register_filesystem
            |-> shmem_init
                |-> kern_mount
                    |-> vfs_kern_mount
                        |-> mount_fs
            |-> init_ramfs_fs
        |-> init_mount_tree
            |-> vfs_kern_mount
            |-> create_mnt_ns
            |-> set_fs_pwd
            |-> set_fs_root
    |-> bdev_cache_init();
    |-> chrdev_init();

...

|-> rest_init
    |-> kernel_init
        |-> kernel_init_freeable
            |-> do_basic_setup
                |-> do_initcalls
                    |-> initcall_levels:
                        |-> rootfs_initall(populate_rootfs)
                            |-> //Unpacking initramfs...
                            |-> unpack_to_rootfs
            |-> prepare_namespace
                |-> mount_root
                    |-> mount_nfs_root
                    |-> mount_block_root
                        |-> do_mount_root
                            |-> sys_mount = SYSCALL mount
                                |-> do_mount
                                    |-> do_remount
                                    |-> do_loopback
                                    |-> do_change_type
                                    |-> do_move_mount
                                    |-> do_new_mount
                                        |-> vfs_kern_mount
                                        |-> do_add_mount
```
