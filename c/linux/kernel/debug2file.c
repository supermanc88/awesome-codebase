#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/stat.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/rtc.h>


/**
 * @brief 用来打印调试日志到文件
 *
 * @param fmt
 * @param ...
 */
void custom_debug_print(const char *fmt, ...)
{
	char dbg_buf[1024] = {0};

	// 获取当前时间
	struct timeval now;
	do_gettimeofday(&now);
	struct rtc_time tm = {0};
	rtc_time_to_tm(now.tv_sec, &tm);
	sprintf(dbg_buf, "[%d-%d-%d %d:%d:%d] ",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec);

	va_list args;
	va_start(args, fmt);
	vsprintf(dbg_buf + strlen(dbg_buf), fmt, args);
	va_end(args);

	struct file *fp = NULL;
    fp = filp_open("/var/log/infosecfs_debug.log", O_CREAT | O_RDWR | O_APPEND, 0777);
    if (!fp) {
        printk("file_open file failed\n");
        return;
    }

    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

	loff_t offset = fp->f_path.dentry->d_inode->i_size;
    ret = vfs_write(fp, dbg_buf, strlen(dbg_buf), &offset);

    set_fs(oldfs);

    filp_close(fp, NULL);
}

int __init write2file_init(void)
{
	custom_debug_print("test\n");
	custom_debug_print("aaaaa%d\n", 11);
	printk("write2file init\n");
	return -1;
}

void __exit write2file_exit(void)
{
	printk("write2file exit\n");

}


module_init(write2file_init);
module_exit(write2file_exit);
MODULE_LICENSE("GPL");
