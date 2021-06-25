#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>


/**
* \require <linux/time.h>
* \brief gettime1 使用 do_gettimeofday 在内核版本5中已不在支持
*/
void gettime1(void)
{
    struct timeval now;

    do_gettimeofday(&now);

    // kern_time_init timeval struct size = 16, sec size = 8, usec size = 8

    printk("%s timeval struct size = %llu, sec size = [ %llu ], usec size = [ %llu ]\n", __func__, sizeof (now), sizeof (now.tv_sec), sizeof (now.tv_usec));

    printk("%s current time = %lu.%lu(secs.usecs)\n", __func__, now.tv_sec, now.tv_usec);
}


/**
* \require <linux/time.h>
* \brief gettime2 这个函数是相对来说比较通用的，在内核3和5中都可使用
*/
void gettime2(void)
{
	struct timespec64 now = {0};

	ktime_get_ts64(&now);

	printk("%s struct size = [ %llu ], sec = [%lld], nsec = [%ld]\n", __func__, sizeof(now), now.tv_sec, now.tv_nsec);
}


/**
* \require <linux/time.h>
* \brief kern_time_init
* \return
*/
static int __init kern_time_init(void)
{
	gettime1();

    return 0;
}


static void __exit kern_time_exit(void)
{
    printk("kern_time_exit");
}



module_init(kern_time_init);
module_exit(kern_time_exit);

MODULE_LICENSE("GPL");
