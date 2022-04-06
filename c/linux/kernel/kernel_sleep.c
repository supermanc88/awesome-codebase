#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/delay.h>


int __init kernel_sleep_test_init(void)
{
    int ret = 0;
    printk("%s\n", __func__);

    // ssleep 和 msleep不占用CPU时间，它slppe的时候，其他模块可以使用此CPU
    ssleep(10);

    printk("%s sleep end\n", __func__);

    return -1;
}


void __exit kernel_sleep_test_exit(void)
{
    printk("%s\n", __func__);
}


module_init(kernel_sleep_test_init);
module_exit(kernel_sleep_test_exit);
MODULE_LICENSE("GPL");