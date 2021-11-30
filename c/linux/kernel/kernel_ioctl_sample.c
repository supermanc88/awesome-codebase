#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define IOCTL_MAGIC 'k'
#define IOCTL_GET_INT          _IOR(IOCTL_MAGIC, 0, int)
#define IOCTL_SET_INT          _IOW(IOCTL_MAGIC, 1, int)
#define IOCTL_GET_STRING       _IOR(IOCTL_MAGIC, 2, char *)
#define IOCTL_SET_STRING       _IOW(IOCTL_MAGIC, 3, char *)
#define IOCTL_MSG              _IO(IOCTL_MAGIC, 4)

dev_t dev_num;
struct cdev *cdev;
struct class *module_class;

long kernel_ioctl_sample_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int a = 10;
    char s[100] = "Hello World";
    printk(KERN_INFO "kernel_ioctl_sample_unlocked_ioctl\n");
    switch (cmd) {
        case IOCTL_GET_INT:
            copy_to_user((int *)arg, &a, sizeof(int));
            printk(KERN_INFO "IOCTL_GET_INT a = %d\n", a);
            break;
        case IOCTL_SET_INT:
            copy_from_user(&a, (int *)arg, sizeof(int));
            printk(KERN_INFO "IOCTL_SET_INT a = %d\n", a);
            break;
        case IOCTL_GET_STRING:
            copy_to_user((char *)arg, s, strlen(s));
            printk(KERN_INFO "IOCTL_GET_STRING s = %s\n", s);
            break;
        case IOCTL_SET_STRING:
            copy_from_user(s, (char *)arg, strlen(arg));
            printk(KERN_INFO "IOCTL_SET_STRING s = %s\n", s);
            break;
        case IOCTL_MSG:
            printk(KERN_INFO "IOCTL_MSG\n");
            break;
    }

    return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = kernel_ioctl_sample_unlocked_ioctl,
};

int __init kernel_ioctl_sample_init(void)
{
    printk(KERN_INFO "Hello, world\n");
    int ret = alloc_chrdev_region(&dev_num, 0, 1, "kernel_ioctl_sample_dev");
    if (ret) {
        printk(KERN_INFO "alloc_chrdev_region failed\n");
        goto error1;
    }

    cdev = cdev_alloc();
    if (cdev == NULL) {
        printk(KERN_INFO "cdev_alloc failed\n");
        goto error2;
    }
    cdev_init(cdev, &fops);

    ret = cdev_add(cdev, dev_num, 1);
    if (ret) {
        printk(KERN_INFO "cdev_add failed\n");
        goto error3;
    }
    module_class = class_create(THIS_MODULE, "kernel_ioctl_sample_class");
    device_create(module_class, NULL, dev_num, NULL, "kernel_ioctl_sample_dev");

    return 0;
error3:
    kfree(cdev);
error2:
    unregister_chrdev_region(dev_num, 1);
error1:
    return ret;
}


void __exit kernel_ioctl_sample_exit(void)
{
    printk(KERN_INFO "Goodbye, world\n");
    kfree(cdev);
    unregister_chrdev_region(dev_num, 1);
    device_destroy(module_class, dev_num);
    class_destroy(module_class);
}


module_init(kernel_ioctl_sample_init);
module_exit(kernel_ioctl_sample_exit);
MODULE_LICENSE("GPL");