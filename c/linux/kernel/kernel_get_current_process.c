#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fs.h>

/*
static __always_inline struct task_struct *get_current(void)
{
	return percpu_read_stable(current_task);
}

#define current get_current()
*/

/**
* \brief infosecfs_get_process_full_path 获取进程的全路径
* \param pid current->pid
* \param proc_path_buf
* \param buf_size
* \return buf pointer
*/
char *get_process_full_path(pid_t pid, char *proc_path_buf, int buf_size)
{
	struct file *process_file;
	char *process_path = NULL;
	char proc_buf[256] = {0};
	sprintf(proc_buf, "/proc/%d/exe", pid);
	process_file = filp_open(proc_buf, 0, 0);
	path_get(&process_file->f_path);
	process_path = d_path(&process_file->f_path, proc_path_buf, buf_size);
	path_put(&process_file->f_path);
	filp_close(process_file, NULL);

	return process_path;
}


/**
 * @brief Get the current process full path object，上面的函数当遇到
 *       /proc/pid/exe 无链接指向时，它会崩溃，所以用mm->exe_file判断一下
 * 
 * @param proc_path_buf 
 * @param buf_size 
 * @return char* 
 */
char *get_current_process_full_path(char *proc_path_buf, int buf_size)
{
    struct mm_struct *mm = current->mm;
    char *process_path = NULL;
    if (mm) {
        down_read(&mm->mmap_sem);

        if (mm->exe_file) {
            process_path = d_path(&mm->exe_file->f_path, proc_path_buf, buf_size);
        }

        up_read(&mm->mmap_sem);
    }
    return process_path;
}


static int __init get_current_process_init(void)
{
    printk(KERN_ERR "get_current_process_init\n");

    int cur_pid = current->pid;

    char proc_path_buf[256] = {0};
    char *proc_path = get_process_full_path(cur_pid, proc_path_buf, 256);
    printk(KERN_DEBUG "get_current_process_init, proc_path = %s\n", proc_path);

    struct task_struct *parent = current->parent;

    int par_pid = parent->pid;

    printk(KERN_DEBUG "get_current_process_init, pid = %d, com = %s\n", cur_pid, current->comm);
    printk(KERN_DEBUG "get_current_process_init, par_pid = %d, par_com = %s\n", par_pid, parent->comm);

    // 返回-1，就不用总是卸载模块了，测试时这样写
    return -1;
}


static void __exit get_current_process_exit(void)
{
    printk(KERN_ERR "get_current_process_exit\n");
}


module_init(get_current_process_init);
module_exit(get_current_process_exit);
MODULE_LICENSE("GPL");