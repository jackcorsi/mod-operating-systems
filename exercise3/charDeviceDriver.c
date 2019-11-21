#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#define DEBUG_MSGS
#define DEVICE_NAME "opsysmem"
MODULE_LICENSE("GPL");

static long device_ioctl(struct file *fp, unsigned int num, unsigned long param);
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *fp, char __user *buffer, size_t length, loff_t *offset);
static ssize_t device_write(struct file *fp, const char __user *buffer, size_t length, loff_t *offset);

//The file operations we're supporting on this driver, and their implementations
const struct file_operations FILE_OPERATIONS = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .unlocked_ioctl = device_ioctl,
        .release = device_release
};

DEFINE_MUTEX(mutex);

typedef struct message_t {
    volatile struct list_head hd;
    volatile size_t size;
    volatile char data[]; //C99 "flexible array member"
} message_t;

volatile struct list_head msgs_hd;
const size_t MAX_MESSAGE_SIZE = 4 * 1024;
volatile unsigned long current_total_size = 0;
volatile unsigned long max_total_size = 2 * 1024 * 1024;
int major;

int init_module() {
    major = register_chrdev(0, DEVICE_NAME, &FILE_OPERATIONS);

    if (major < 0) {
        printk(KERN_ERR "Registering char device failed with code %d", major);
        return major;
    }

    //Copied to make sure the tests work
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    INIT_LIST_HEAD(&msgs_hd);
    return 0;
}

void cleanup_module() {
    printk(KERN_INFO "device cleanup\n");
    unregister_chrdev(major, DEVICE_NAME);

    mutex_lock(&mutex); //Stop all operations
    if (!list_empty(&msgs_hd)) { //Free the messages
        struct list_head *h = msgs_hd.next;
        while (h != &msgs_hd) {
            struct list_head *next = h->next;
            message_t *msg = list_entry(h, message_t, hd);
            kfree(msg);
            h = next;
        }
    }
}

static int device_open(struct inode * inode, struct file *file) {
    printk(KERN_INFO "device opened\n");
    return 0;
}

static ssize_t device_read(struct file *fp, char __user *buffer, size_t length, loff_t *offset) {
    //fp and offset not used
    message_t *msg = NULL;

    mutex_lock(&mutex);
    if (!list_empty(&msgs_hd)) {
        struct list_head *msg_hd = msgs_hd.next;
        list_del(msg_hd);
        msg = list_entry(msg_hd, message_t, hd);
        current_total_size -= msg->size;
    }
    mutex_unlock(&mutex);

    if (!msg)
        return -EAGAIN;

    if (length > msg->size) { //Don't allow arbitrary reading
#ifdef DEBUG_MSGS
        printk(KERN_DEBUG "user requested more data than available\n");
#endif
        length = msg->size;
    }

#ifdef DEBUG_MSGS
    if (length < msg->size)
        printk(KERN_DEBUG "User requested less data than the message size\n");
#endif

    if (copy_to_user(buffer, &(msg->data), length)) {
        printk(KERN_ERR "device_read: Failed to write to userspace buffer\n");
        kfree(msg);
        return -EFAULT;
    } else {
        kfree(msg);
        return length;
    }
}

static ssize_t device_write(struct file *fp, const char __user *buffer, size_t length, loff_t *offset) {
    //fp and offset not used
    if (length > MAX_MESSAGE_SIZE)
        return -EINVAL;

    if (current_total_size + length > max_total_size)
        return -EAGAIN;

    message_t *msg = kmalloc(sizeof(message_t) + length, GFP_KERNEL);
    if (copy_from_user(&(msg->data), buffer, length)) {
        printk(KERN_ERR "device_write: Failed to read from userspace buffer\n");
        kfree(msg);
        return -EFAULT;
    }
    INIT_LIST_HEAD(&(msg->hd));
    msg->size = length;

    int error = 1;

    mutex_lock(&mutex);
    if (current_total_size + length <= max_total_size) { //Since we were unlocked before, this could have changed
        list_add_tail(&(msg->hd), &msgs_hd);
        current_total_size += length;
        error = 0;
    }
    mutex_unlock(&mutex);

    if (error) {
#ifdef DEBUG_MSGS
        printk(KERN_DEBUG "device_write: Not enough room after locking");
        kfree(msg);
        return -EAGAIN;
#endif
    }

    return length;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "device closed\n");
    return 0;
}

static long device_ioctl(struct file *fp, unsigned int num, unsigned long param) {
    if (num != 0) {
        printk(KERN_ERR "Unknown ioctl number\n");
        return -EINVAL;
    }

    int return_val;

    mutex_lock(&mutex);
    if (param >= current_total_size) {
        max_total_size = param;
        return_val = 0;
        printk(KERN_INFO "Max total data set to %lu\n", param);
    } else {
        return_val = -EINVAL;
    }
    mutex_unlock(&mutex);

    return return_val;
}
#pragma clang diagnostic pop