#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>



/**
* \require <linux/time.h>
* \brief kern_time_init
* \return
*/
static int __init kern_time_init(void)
{

    struct timeval now;

    do_gettimeofday(&now);

    // kern_time_init timeval struct size = 16, sec size = 8, usec size = 8

    printk("%s timeval struct size = %d, sec size = %d, usec size = %d\n", __func__, sizeof (now), sizeof (now.tv_sec), sizeof (now.tv_usec));

    printk("%s current time = %lu.%lu(secs.usecs)\n", __func__, now.tv_sec, now.tv_usec);

    return 0;
}


static void __exit kern_time_exit(void)
{
    printk("kern_time_exit");
}



module_init(kern_time_init);
module_exit(kern_time_exit);

MODULE_LICENSE("GPL");
