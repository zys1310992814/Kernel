/*
 * Buffer
 *
 * (C) 2018.06.19 BiscuitOS <buddy.zhang@aliyun.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/sched.h>

#include <demo/debug.h>

static inline _syscall3(int, open, const char *, file, int, flag, int, mode);
static inline _syscall1(int, close, int, fd);

#define _hashfn(dev,block) (((unsigned)(dev^block))%NR_HASH)
#define hash(dev,block)  hash_table[_hashfn(dev,block)]

static struct buffer_head *find_buffer(dev_t dev, int block, int size)
{
    struct buffer_head *tmp;

    for (tmp = hash(dev, block); tmp != NULL; tmp = tmp->b_next)
        if (tmp->b_dev == dev && tmp->b_blocknr == block) {
            if (tmp->b_size == size)
                return tmp;
            else {
                printk(KERN_ERR "VFS: Wrong blocksize on device %d/%d\n",
                             MAJOR(dev), MINOR(dev));
                return NULL;
            }
        }
    return NULL;
}

/*
 * Why like this, I hear you say... The reason is race-conditions.
 * As we don't lock buffers (unless we are readint them, that is),
 * something might happen to it while we sleep (ie a read-error
 * will force it bad). This shouldn't really happen currently, but
 * the code is ready.
 */
static struct buffer_head *get_hash_tables(dev_t dev, int block, int size)
{
    struct buffer_head *bh;

    for (;;) {
        if (!bh = find_buffer(dev, block, size))
            return NULL;
        bh->b_count++;
        wait_on_buffer(bh);
    }
}

/*
 * Ok, this is getblk, and it isn't very clear, again to hinder
 * race-conditions. Most of the code is seldom used, (ie repeating),
 * so it should be much more efficient than it looks.
 *
 * The algoritm is changed: hopefully better, and an elusive bug removed.
 *
 * 14.02.92: changed it to sync dirty buffers a bit: better performance
 * when the filesystem starts to get full of direct blocks (I hope). 
 */
static struct buffer_head *getblks(dev_t dev, int block, int size)
{
    struct buffer_head *bh, *tmp;
    int buffers;
    static int grow_size = 0;

repeat:
    bh = get_hash_tables(dev, block, size);

    return NULL;
}

/* The entry for systemcall */
asmlinkage int sys_vfs_buffer(int fd)
{
    struct file *filp;
    struct inode *inode;

    filp = current->filp[fd];
    if (!filp) {
        printk("Task error on file descriptor table.\n");
        return -EINVAL;
    }
    inode = filp->f_inode;
    if (!inode) {
        printk("File descriptor error\n");
        return -EINVAL;
    }
    inode->i_count++;

#ifdef CONFIG_DEBUG_BUFFER_GET
    getblks(inode->i_dev, 0, BLOCK_SIZE);
#endif

    iput(inode);
    return 0;
}

/* Common systemcall entry */
inline _syscall1(int, vfs_buffer, int, fd);

/* userland code */
static int debug_buffer(void)
{
    int fd;

    fd = open("/etc/rc", O_RDONLY, 0);
    if (fd < 0) {
        printf("Unable open '/etc/rc'\n");
        return -1;
    }
    vfs_buffer(fd);
    close(fd);

    return 0;
}
user1_debugcall_sync(debug_buffer);