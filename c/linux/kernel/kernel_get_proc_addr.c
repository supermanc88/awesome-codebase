#include <linux/kallsyms.h>
#include <linux/kernel.h>

typedef unsigned long (*kallsyms_lookup_name_fn)(const char *name);

kallsyms_lookup_name_fn kallsyms_lookup_name_addr = NULL;


/**
* \require <linux/kallsyms.h> 未导出kallsyms_lookup_name，但导出了kallsyms_on_each_symbol
* \brief find_fn
* \param data
* \param name
* \param mod
* \param addr
* \return
*/
int find_fn(void *data, const char *name, struct module *mod, unsigned long addr)
{
    if (name != NULL && strcmp(name, "kallsyms_lookup_name") == 0) {
        kallsyms_lookup_name_addr = addr;
        return 1;
    }
    return 0;
}


static int __init get_syms_init(void)
{
    unsigned long do_exit_addr, profile_task_exit_addr;

    printk("%s\n", __func__);


	// 获取 kallsyms_lookup_name 函数地址
    kallsyms_on_each_symbol(find_fn, 0);

    if (kallsyms_lookup_name_addr == NULL) {
        printk("%s cant get kallsyms_lookup_name addr\n", __func__);
        return 0;
    }

    printk("%s kallsyms_lookup_name addr = [%p]\n", __func__, kallsyms_lookup_name_addr);

    profile_task_exit_addr = kallsyms_lookup_name_addr("profile_task_exit");

    do_exit_addr = kallsyms_lookup_name_addr("do_exit");


    printk("%s profile_task_exit addr = [%p], do_exit addr = [%p]\n", __func__, profile_task_exit_addr, do_exit_addr);

    return 0;
}



static void __exit get_syms_exit(void)
{
    printk("%s\n", __func__);
}

module_init(get_syms_init);
module_exit(get_syms_exit);
MODULE_LICENSE("GPL");
