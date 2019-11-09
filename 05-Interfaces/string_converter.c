// SPDX-License-Identifier: GPL
#include <linux/init.h>     // Macros used to mark up functions  __init __exit
#include <linux/module.h>   // Core header for loading LKMs into the kernel
#include <linux/kernel.h>   // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>

#define PROC_DIR "string_converter"
#define PROC_FILE "pfile"
#define PROC_STAT "stat"

static char pmsg[PAGE_SIZE];
static ssize_t p_msg_size;

struct statistic {
	int32_t numb_rd_call;
	int32_t numb_wr_call;
	int32_t numb_conv;
};

static struct statistic pstat;

static void conv_uppercase(char *str, size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		if (str[i] >= 'a' && str[i] <= 'z') {
			str[i] += 'A' - 'a';
			pstat.numb_conv++;
		}
	}

}

static ssize_t pwrite(struct file *file, const char __user *pbuf,
	size_t count, loff_t *ppos)
{
	ssize_t not_copied;

	pr_info("string_converter: %s: count=%d\n", __func__, count);

	not_copied = copy_from_user(pmsg, pbuf, count);

	p_msg_size = count - not_copied;

	conv_uppercase(pmsg, p_msg_size);

	pstat.numb_wr_call++;
	pr_info("string_converter: %s: p_msg_size=%d\n", __func__, p_msg_size);
	return p_msg_size;
}

static ssize_t pread(struct file *file, char __user *pbuf, size_t count,
	loff_t *ppos)
{
	static ssize_t num, not_copied;

	pr_info("string_converter: %s: count=%d\n", __func__, count);
	num = min_t(ssize_t, p_msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, pmsg, num);
		num -= not_copied;
		pstat.numb_rd_call++;
	}
	p_msg_size = 0;

	pr_info("string_converter: %s: return= %d\n", __func__, num);
	return num;
}

static ssize_t read_stat(struct file *file, char __user *pbuf,
	size_t count, loff_t *ppos)
{
	static ssize_t num, not_copied, size_stat;
	static int32_t numb_call_rd_stat;

	num = 0;
	pr_info("string_converter: numb_call_rd_stat %d\n", numb_call_rd_stat);
	if ((numb_call_rd_stat % 3) == 0) {
		char stat[200];

		size_stat = sprintf(stat, "Proc statistics:\n"
			"Read calls: %d\n"
			"Write calls: %d\n"
			"Numb convert to upper: %d\n",
			pstat.numb_rd_call, pstat.numb_wr_call,
			pstat.numb_conv);
		num = min_t(ssize_t, size_stat, count);
		if (num) {
			not_copied = copy_to_user(pbuf, stat, num);
			num -= not_copied;
		}
		pr_info("string_converter: %s: %d bytes\n", __func__, num);
	}
	numb_call_rd_stat++;
	return num;
}
static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = pread,
	.write = pwrite,
};
static const struct file_operations stat_file = {
	.owner = THIS_MODULE,
	.read = read_stat,
};

static struct proc_dir_entry *pdir;
static struct proc_dir_entry *pfile, *stfile;

static int __init conv_init(void)
{
	pdir = proc_mkdir(PROC_DIR, NULL);
	if (pdir == NULL) {
		pr_err("string_converter: error creating procfs dir\n");
		return -ENOMEM;
	}

	pfile = proc_create(PROC_FILE, 0666, pdir, &myops);
	if (pfile == NULL) {
		pr_err("string_converter: error creating procfs file\n");
		proc_remove(pdir);
		return -ENOMEM;
	}

	stfile = proc_create(PROC_STAT, 0666, pdir, &stat_file);
	if (stfile == NULL) {
		pr_err("string_converter: error creating procfs file\n");
		proc_remove(pdir);
		return -ENOMEM;
	}

	pr_info("string_converter: loaded\n");
	return 0;
}

static void __exit conv_exit(void)
{
	proc_remove(pdir);
	pr_info("string_converter: closed");
}

module_init(conv_init);
module_exit(conv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Happybolt <dmytro.topikha@gmail.com>");
MODULE_DESCRIPTION("String convert for linux kernel");
MODULE_VERSION("0.1");
