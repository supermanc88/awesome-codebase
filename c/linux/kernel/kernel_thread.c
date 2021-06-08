#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

struct task_struct *test_task;

static int thread_func(void *data)
{
	int a = 1;
	// 线程框架
	while (true) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (kthread_should_stop())
			break;

		if ( a ) {
			// 条件为真
			// 2. 进行业务处理
		} else {
			// 条件为假
			// 让出CPU运行其他线程，并存指定的时间内重新被调度
			// 4. 线程函数必须能让出cpu，以便能运行其他线程。
			// 同时线程函数也必须能重新被调度运行。
			schedule_timeout(HZ);
		}
	}
	return 0;
}

static int __init kernel_thread_test_init(void)
{
	int ret = 0;
	// 1. 线程创建后，不会马上运行，需要wake_up_process运行
	// 或直接使用 kthread_run
	test_task = kthread_create(thread_func, NULL, "test_task");

	if (!IS_ERR(test_task)) {
		wake_up_process(test_task);
	}
	/** test_task = kthread_run(thread_func, NULL, "test_task"); */
	printk("%s\n", __func__);
	return ret;
}

static void __exit kernel_thread_test_exit(void)
{
	printk("%s\n", __func__);
	if (test_task) {
		// 3. 在调用kthread_stop函数时，线程函数不能已经结束运行。
		// 否则，kthread_stop函数会一直等待下去。
		kthread_stop(test_task);
		test_task = NULL;
	}
}


module_init(kernel_thread_test_init);
module_exit(kernel_thread_test_exit);
MODULE_LICENSE("GPL");
