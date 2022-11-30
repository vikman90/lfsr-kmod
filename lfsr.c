/**
 * @file lfsr.c
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief LFSR kernel module
 * @version 0.5
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define LFSR_DEVICE_NAME "lfsr"
#define HALF_SHIFT (sizeof(lfsr_t) * 4)

typedef uint64_t lfsr_t;

static ssize_t lfsr_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t lfsr_write(struct file *, const char __user *, size_t, loff_t *);
static int lfsr_open(struct inode *, struct file *);
static int lfsr_release(struct inode *, struct file *);
static void lfsr_shift(void);

DEFINE_MUTEX(lfsr_mutex);

static lfsr_t lfsr_state;

static struct file_operations lfsr_fops = {
    read: lfsr_read,
    write: lfsr_write,
    open: lfsr_open,
    release: lfsr_release
};

static int lfsr_major;
static struct class * lfsr_cls;

static int __init lfsr_init(void) {
    lfsr_state = ktime_get_real();
    lfsr_major = register_chrdev(0, LFSR_DEVICE_NAME, &lfsr_fops);

    if (lfsr_major < 0) {
        pr_alert("lfsr_init(): register_chrdev(): %d\n", lfsr_major);
        return 1;
    }

    lfsr_cls = class_create(THIS_MODULE, LFSR_DEVICE_NAME);
    device_create(lfsr_cls, NULL, MKDEV(lfsr_major, 0), NULL, LFSR_DEVICE_NAME);

    pr_info(LFSR_DEVICE_NAME ": initialized\n");
    return 0;
}

static void __exit lfsr_exit(void) {
    device_destroy(lfsr_cls, MKDEV(lfsr_major, 0));
    class_destroy(lfsr_cls);
    unregister_chrdev(lfsr_major, LFSR_DEVICE_NAME);

    pr_info(LFSR_DEVICE_NAME ": exited\n");
}

ssize_t lfsr_read(struct file * filep, char __user * buffer, size_t size, loff_t * offset) {
    ssize_t retval = size;
    size_t length;
    char * kptr;
    char * kbuffer = kmalloc(size, GFP_KERNEL);

    if (kbuffer == NULL) {
        pr_err("lfsr_read: kmalloc returned NULL.");
        return -ENOMEM;
    }

    mutex_lock(&lfsr_mutex);

    for (length = size, kptr = kbuffer; length >= sizeof(lfsr_state); length -= sizeof(lfsr_state), kptr += sizeof(lfsr_state)) {
        *(lfsr_t *)kptr = lfsr_state;
        lfsr_shift();
    }

    if (length > 0) {
        memcpy(kptr, &lfsr_state, length);
        lfsr_shift();
    }

    mutex_unlock(&lfsr_mutex);

    if (copy_to_user(buffer, kbuffer, size) != 0) {
        retval = -EFAULT;
    }

    kfree(kbuffer);

    return retval;
}

ssize_t lfsr_write(struct file * filep, const char __user * buffer, size_t size, loff_t * offset) {
    char * kbuffer = kmalloc(size, GFP_KERNEL);

    if (kbuffer == NULL) {
        pr_err("lfsr_write: kmalloc returned NULL.");
        return -ENOMEM;
    }

    if (copy_from_user(kbuffer, buffer, size) != 0) {
        kfree(kbuffer);
        return -EFAULT;
    }

    mutex_lock(&lfsr_mutex);

    for (size_t length = size; length >= sizeof(lfsr_state); length -= sizeof(lfsr_state), kbuffer += sizeof(lfsr_state)) {
        lfsr_state ^= *(lfsr_t *)kbuffer;
    }

    mutex_unlock(&lfsr_mutex);
    kfree(kbuffer);

    return size;
}

int lfsr_open(struct inode * inode, struct file * filep) {
    try_module_get(THIS_MODULE);
    return 0;
}

int lfsr_release(struct inode * inode, struct file * filep) {
    module_put(THIS_MODULE);
    return 0;
}

void lfsr_shift(void) {
    for (int i = 0; i < 2; i++) {
        lfsr_t next = lfsr_state ^ (lfsr_state >> 2) ^ (lfsr_state >> 3) ^ (lfsr_state >> 5);
        lfsr_state = (lfsr_state >> HALF_SHIFT) | (next << HALF_SHIFT);
    }
}

module_init(lfsr_init);
module_exit(lfsr_exit);

MODULE_LICENSE("GPL");
