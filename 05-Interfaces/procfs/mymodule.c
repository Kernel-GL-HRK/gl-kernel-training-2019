#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

#define DIR_NAME	"my_module"
#define CONV_FILE_NAME	"string-conv"
#define LOG_FILE_NAME	"log"

static struct proc_dir_entry *dir;
static struct proc_dir_entry *fstr, *flog;

static ssize_t myread(struct file *file, char __user *pbuf, size_t count,
	loff_t *ppos)
{
	return 0;
}

static ssize_t mywrite(struct file *file, const char __user *pbuf,
	size_t count, loff_t *ppos)
{
	return -EROFS;
}

static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static int __init init_mod(void)
{
	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("mymodule-procfs: error creating procfs directory\n");
		return -ENOMEM;
	}

	fstr = proc_create(CONV_FILE_NAME, 0666, dir, &myops);
	if (fstr == NULL) {
		pr_err("mymodule-procfs: error creating procfs entry 1\n");
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	flog = proc_create(LOG_FILE_NAME, 0666, dir, &myops);
	if (flog == NULL) {
		pr_err("mymodule-procfs: error creating procfs entry 2\n");
		remove_proc_entry(CONV_FILE_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("mymodule-procfs: module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	proc_remove(dir);
	pr_info("mymodule-procfs: module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");
