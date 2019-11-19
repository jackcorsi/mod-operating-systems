#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "mod-os-message-queue"
MODULE_LICENSE("GPL");

long device_ioctl(struct file *fp, unsigned int num, unsigned long param);
int init_module(void);
void cleanup_module(void);
int device_open(struct inode *inode, struct file *file);
int device_release(struct inode *inode, struct file *file);
ssize_t device_read(struct file *fp, char *buffer, size_t length, loff_t *offset);
ssize_t device_write(struct file *fp, const char *buffer, size_t length, loff_t *offset);


//The file operations we're supporting on this driver, and their implementations
const struct file_operations FILE_OPERATIONS = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .unlocked_ioctl = device_ioctl,
        .release = device_release
};

int major;

int init_module() {
    major = register_chrdev(0, DEVICE_NAME, &FILE_OPERATIONS);

    if (major < 0) {
        printk(KERN_ERR "Registering char device failed with code %d", major);
        return major;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to the driver, create a dev file"
                     " with e.g. mknod /dev/%s c %d 0\n remember to remove the file and module later\n",
                     major, DEVICE_NAME, major);

    return 0;
}

void cleanup_module() {
    printk(KERN_DEBUG "cleanup executed\n");
    unregister_chrdev(major, DEVICE_NAME);
}

int device_open(struct inode * inode, struct file *file) {
    printk(KERN_ERR "Not implemented yet\n");
    return -EINVAL;
}

ssize_t device_read(struct file *fp, char *buffer, size_t length, loff_t *offset) {
    printk(KERN_ERR "Not implemented yet\n");
    return -EINVAL;
}

ssize_t device_write(struct file *fp, const char *buffer, size_t length, loff_t *offset) {
    printk(KERN_ERR "Not implemented yet\n");
    return -EINVAL;
}

int device_release(struct inode *inode, struct file *file) {
    printk(KERN_ERR "Not implemented yet\n");
    return -EINVAL;
}

long device_ioctl(struct file *fp, unsigned int num, unsigned long param) {
    printk(KERN_ERR "Not implemented yet\n");
    return -EINVAL;
}