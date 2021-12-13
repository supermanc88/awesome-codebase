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


int read_write_file_test(void)
{
	struct file *fp1 = NULL;
	struct file *fp2 = NULL;

	unsigned char buf[1024] = {0};

	fp1 = file_open("/testfile", O_RDWR, 0666);
	fp2 = file_open("/backupfile", O_RDWR | O_CREAT, 0666);

	if (!fp1 || !fp2) {
		printk("file_open file failed\n");
		return -1;
	}

	int offset = 0;
	int read_size = 0;
	while ( (read_size = file_read(fp1, offset, buf, 1024)) > 0 ) {
		file_write(fp2, offset, buf, read_size);
		offset += read_size;
	}

	file_close(fp1);
	file_close(fp2);

	return 0;
}


static int dev_mkdir(const char *name, mode_t mode)
{
	struct nameidata nd;
	struct nameidata root_nd;
	struct dentry *dentry;
	int err;

    err = path_lookup("/", LOOKUP_DIRECTORY, &root_nd);
    if (IS_ERR(err)) {
        printk(KERN_ERR "path_lookup: failed, error = %d\n", err);
        return err;
    }

	err = vfs_path_lookup(root_nd.root.dentry, root_nd.root.mnt, name, LOOKUP_PARENT, &nd);
    if (IS_ERR(err)) {
        printk(KERN_ERR "vfs_path_lookup: failed, error = %d\n", err);
        return err;
    }

	dentry = lookup_create(&nd, 1);
	if (!IS_ERR(dentry)) {
		err = vfs_mkdir(nd.path.dentry->d_inode, dentry, mode);
		dput(dentry);
	} else {
		err = PTR_ERR(dentry);
	}
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);

	path_put(&nd.path);
	return err;
}

static int create_path(const char *nodepath)
{
	char *path;
	struct nameidata nd;
	int err = 0;

	path = kstrdup(nodepath, GFP_KERNEL);
	if (!path)
		return -ENOMEM;

	err = path_lookup(path, LOOKUP_PARENT, &nd);
	if (err == 0) {
		struct dentry *dentry;

		/* create directory right away */
		dentry = lookup_create(&nd, 1);
		if (!IS_ERR(dentry)) {
			err = vfs_mkdir(nd.path.dentry->d_inode,
					dentry, 0755);
			dput(dentry);
		}
		mutex_unlock(&nd.path.dentry->d_inode->i_mutex);

		path_put(&nd.path);
	} else if (err == -ENOENT) {
		char *s;

		/* parent directories do not exist, create them */
		s = path;
		while (1) {
			s = strchr(s, '/');
			if (!s)
				break;
			s[0] = '\0';
			err = dev_mkdir(path, 0755);
            printk(KERN_INFO "dev_mkdir %s, err %d\n", path, err);
			if (err && err != -EEXIST)
				break;
			s[0] = '/';
			s++;
		}
	}

	kfree(path);
	return err;
}


static int dev_rmdir(const char *name)
{
	struct nameidata nd;
	struct nameidata root_nd;
	struct dentry *dentry;
	int err;

    err = path_lookup("/", LOOKUP_DIRECTORY, &root_nd);
    if (err) {
        printk(KERN_ERR "path_lookup: failed, error = %d\n", err);
        return err;
    }

	err = vfs_path_lookup(root_nd.root.dentry, root_nd.root.mnt,
			      name, LOOKUP_PARENT, &nd);
	if (err)
		return err;

	mutex_lock_nested(&nd.path.dentry->d_inode->i_mutex, I_MUTEX_PARENT);
	dentry = lookup_one_len(nd.last.name, nd.path.dentry, nd.last.len);
	if (!IS_ERR(dentry)) {
		if (dentry->d_inode)
			err = vfs_rmdir(nd.path.dentry->d_inode, dentry);
		else
			err = -ENOENT;
		dput(dentry);
	} else {
		err = PTR_ERR(dentry);
	}
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);

	path_put(&nd.path);
	return err;
}

static int delete_path(const char *nodepath)
{
	const char *path;
	int err = 0;

	path = kstrdup(nodepath, GFP_KERNEL);
	if (!path)
		return -ENOMEM;

	while (1) {
		char *base;

		base = strrchr(path, '/');
		if (!base)
			break;
		base[0] = '\0';
		err = dev_rmdir(path);
		if (err)
			break;
	}

	kfree(path);
	return err;
}


int __init kernel_create_file_or_dir_init(void)
{
    // create_file_test();

    // create_path("/444/2222/333/44411/");

	read_write_file_test();

    return -1;
}


void __exit kernel_create_file_or_dir_exit(void)
{
}


module_init(kernel_create_file_or_dir_init);
module_exit(kernel_create_file_or_dir_exit);
MODULE_LICENSE("GPL");