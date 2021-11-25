#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/stat.h>

struct file *file_open(const char *path, int flags, int rights)
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        printk(KERN_ERR "file_open: open file '%s' failed, error = %d\n", path, err);
        return NULL;
    }
    return filp;
}


void file_close(struct file *file)
{
    filp_close(file, NULL);
}


int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}


int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}


int file_sync(struct file *file)
{
    // vfs_fsync(file, 0);
    return 0;
}


int create_dir_test(void)
{
    struct nameidata nd;
    int err = 0;
    err = path_lookup("/", LOOKUP_DIRECTORY, &nd);
    if (IS_ERR(err)) {
        printk("path_lookup failed\n");
        return -1;
    }

    struct dentry *par_dentry;
    par_dentry = nd.path.dentry;

    struct dentry *dentry;
    dentry = d_alloc_name(par_dentry, "1111");

    if (IS_ERR(dentry)) {
        printk("d_alloc_name failed\n");
        return -1;
    }

    err = vfs_mkdir(par_dentry->d_inode, dentry, S_IRWXU | S_IRWXO | S_IRWXG);
    if (IS_ERR(err)) {
        printk("vfs_mkdir failed\n");
        return -1;
    }
    path_put(&nd.path);

    return 0;
}

int create_file_test(void)
{
    struct file *fp = NULL;
    fp = file_open("/testfile", O_CREAT | O_RDWR | O_APPEND, 0777);
    if (!fp) {
        printk("file_open file failed\n");
        return -1;
    }
    file_close(fp);
    return 0;
}


int __init kernel_create_file_or_dir_init(void)
{
    create_dir_test();
    // create_file_test();

    return -1;
}


void __exit kernel_create_file_or_dir_exit(void)
{
}


module_init(kernel_create_file_or_dir_init);
module_exit(kernel_create_file_or_dir_exit);
MODULE_LICENSE("GPL");