#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/err.h>

#include <linux/delay.h>


#include <linux/completion.h>

struct completion g_my_completion;

struct task_struct *g_task1;
struct task_struct *g_task2;


int thread_func1(void *data)
{
	printk("%s enter\n", __func__);
	while (true) {
		if (kthread_should_stop()) {
			break;
		}
		printk("%s\n", __func__);

		wait_for_completion(&g_my_completion);

		printk("%s completion enter\n", __func__);


		// 当完成后，completion就失效了，再次使用需要重新初始化
		init_completion(&g_my_completion);

	}
	printk("%s exit\n", __func__);
	return 0;
}

int thread_func2(void *data)
{
	printk("%s enter\n", __func__);
	while (true) {
		if (kthread_should_stop()) {
			break;
		}
		printk("%s\n", __func__);

		set_current_state(TASK_INTERRUPTIBLE);
		msleep(1000);
		if (!g_my_completion.done) {
			printk("%s set complete\n", __func__);
			complete(&g_my_completion);
		}

	}
	printk("%s exit\n", __func__);
	return 0;
}

static int __init kernel_completion_init(void)
{
	int ret = 0;
	printk("%s\n", __func__);

	// 初始化completion
	init_completion(&g_my_completion);

	g_task1 = kthread_run(thread_func1, NULL, "thread1");
	g_task2 = kthread_run(thread_func2, NULL, "thread2");


	return ret;
}


static void __exit kernel_completion_exit(void)
{
	printk("%s\n", __func__);

	if (g_task1) {
		kthread_stop(g_task1);
		if (!g_my_completion.done) {
			complete(&g_my_completion);
		}
		g_task1 = NULL;
	}
	if (g_task2) {
		kthread_stop(g_task2);
		g_task2 = NULL;
	}
}


module_init(kernel_completion_init);
module_exit(kernel_completion_exit);
MODULE_LICENSE("GPL");
