/**
 * @file lfsr.c
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief LFSR kernel module
 * @version 0.2
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define LFSR_DEVICE_NAME "lfsr"

typedef uint64_t lfsr_t;

static ssize_t lfsr_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t lfsr_write(struct file *, const char __user *, size_t, loff_t *);
static int lfsr_open(struct inode *, struct file *);
static int lfsr_release(struct inode *, struct file *);
static void lfsr_shift(void);

DEFINE_MUTEX(lfsr_mutex);

static union {
    lfsr_t qword;
    struct {
        lfsr_t b0:1;
        lfsr_t b1:1;
        lfsr_t b2:1;
        lfsr_t b3:1;
        lfsr_t b4:1;
        lfsr_t b5:1;
        lfsr_t b6:1;
        lfsr_t b7:1;
        lfsr_t b8:1;
        lfsr_t b9:1;
        lfsr_t b10:1;
        lfsr_t b11:1;
        lfsr_t b12:1;
    } bits;
} lfsr_state;

static struct file_operations lfsr_fops = {
    read: lfsr_read,
    write: lfsr_write,
    open: lfsr_open,
    release: lfsr_release
};

static int lfsr_major;
static struct class * lfsr_cls;

static int __init lfsr_init(void) {
    lfsr_state.qword = ktime_get_real();
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
    size_t length;

    mutex_lock(&lfsr_mutex);

    for (length = size; length >= sizeof(lfsr_state); length -= sizeof(lfsr_state), buffer += sizeof(lfsr_state)) {
        put_user(lfsr_state.qword, (lfsr_t *)buffer);
        lfsr_shift();
    }

    if (length > 0) {
        if (length & 0x4) {
            put_user((uint32_t)lfsr_state.qword, (uint32_t *)buffer);
            buffer += 4;
        }

        if (length & 0x2) {
            put_user((uint16_t)lfsr_state.qword, (uint16_t *)buffer);
            buffer += 2;
        }

        if (length & 0x1) {
            put_user((uint8_t)lfsr_state.qword, (uint8_t *)buffer);
        }

        lfsr_shift();
    }

    mutex_unlock(&lfsr_mutex);

    return size;
}

ssize_t lfsr_write(struct file * filep, const char __user * buffer, size_t size, loff_t * offset) {
    mutex_lock(&lfsr_mutex);

    for (size_t length = size; length >= sizeof(lfsr_state); length -= sizeof(lfsr_state), buffer += sizeof(lfsr_state)) {
        lfsr_t value;
        get_user(value, (lfsr_t *)buffer);
        lfsr_state.qword ^= value;
    }

    mutex_unlock(&lfsr_mutex);

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
    for (size_t i = 0; i < (sizeof(lfsr_state)); i++) {
        uint8_t next = lfsr_state.bits.b0 ^ lfsr_state.bits.b2 ^ lfsr_state.bits.b3 ^ lfsr_state.bits.b5;
        next = (next << 1) | (lfsr_state.bits.b1 ^ lfsr_state.bits.b3 ^ lfsr_state.bits.b4 ^ lfsr_state.bits.b6);
        next = (next << 1) | (lfsr_state.bits.b2 ^ lfsr_state.bits.b4 ^ lfsr_state.bits.b5 ^ lfsr_state.bits.b7);
        next = (next << 1) | (lfsr_state.bits.b3 ^ lfsr_state.bits.b5 ^ lfsr_state.bits.b6 ^ lfsr_state.bits.b8);
        next = (next << 1) | (lfsr_state.bits.b4 ^ lfsr_state.bits.b6 ^ lfsr_state.bits.b7 ^ lfsr_state.bits.b9);
        next = (next << 1) | (lfsr_state.bits.b5 ^ lfsr_state.bits.b7 ^ lfsr_state.bits.b8 ^ lfsr_state.bits.b10);
        next = (next << 1) | (lfsr_state.bits.b6 ^ lfsr_state.bits.b8 ^ lfsr_state.bits.b9 ^ lfsr_state.bits.b11);
        next = (next << 1) | (lfsr_state.bits.b7 ^ lfsr_state.bits.b9 ^ lfsr_state.bits.b10 ^ lfsr_state.bits.b12);

        lfsr_state.qword = (lfsr_state.qword >> 8) | ((lfsr_t)next << 56);
    }
}

module_init(lfsr_init);
module_exit(lfsr_exit);

MODULE_LICENSE("GPL");
