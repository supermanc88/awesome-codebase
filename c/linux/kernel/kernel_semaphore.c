#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/err.h>
#include <linux/kthread.h>

#include <linux/semaphore.h>

#include <linux/delay.h>


// 申请一个信号量
struct semaphore g_sem;

struct task_struct *g_task1;
struct task_struct *g_task2;


int thread_func1(void *data)
{
	while (true) {
		if (kthread_should_stop()) {
			printk("%s thread stop\n", __func__);
			break;
		}

		printk("%s\n", __func__);

		// 获取后会使信号量变量计数减1
		down_interruptible(&g_sem);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(2 * HZ);
		printk("%s get semaphore\n", __func__);

		up(&g_sem);
	}
	return 0;
}

int thread_func2(void *data)
{
	while (true) {
		if (kthread_should_stop()) {
			printk("%s thread stop\n", __func__);
			break;
		}

		printk("%s\n", __func__);

		// 获取后会使信号量变量计数减1
		down_interruptible(&g_sem);

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(5 * HZ);
		printk("%s get semaphore\n", __func__);

		up(&g_sem);

	}
	return 0;
}

static int __init kernel_semaphore_init(void)
{
	int ret = 0;
	printk("%s\n", __func__);

	// 初始化信号量, 信号量变量计数为1
	sema_init(&g_sem, 1);


	g_task1 = kthread_create(thread_func1, NULL, "task1");

	if (!IS_ERR(g_task1)) {
		wake_up_process(g_task1);
	}


	g_task2 = kthread_run(thread_func2, NULL, "task2");

	return ret;
}


static void __exit kernel_semaphore_exit(void)
{
	printk("%s\n", __func__);
	if (g_task1) {
		kthread_stop(g_task1);
	}
	if (g_task2) {
		kthread_stop(g_task2);
	}
}


module_init(kernel_semaphore_init);
module_exit(kernel_semaphore_exit);
MODULE_LICENSE("GPL");
