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

#define SYS_FILE "sfile"
#define SYS_STAT "sstat"

static char pmsg[PAGE_SIZE];
static ssize_t p_msg_size;

static char smsg[PAGE_SIZE];
static ssize_t s_msg_size;

struct statistic {
	int32_t numb_rd_call;
	int32_t numb_wr_call;
	int32_t numb_conv;
};

static struct statistic pstat;

static struct statistic sstat;

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

static void conv_lowercase(char *str, size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] -= 'A' - 'a';
			sstat.numb_conv++;
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

static ssize_t sshow(struct class *class, struct class_attribute *attr,
	char *buf)
{
	pr_info("string_converter: %s: size=%d\n", __func__, s_msg_size);

	sprintf(buf, "%s\n", smsg);
	sstat.numb_rd_call++;

	return s_msg_size;
}

static ssize_t sstore(struct class *class, struct class_attribute *attr,
	const char *buf, size_t count)
{
	sscanf(buf, "%s\n", smsg);
	conv_lowercase(smsg, count);
	s_msg_size = count;
	pr_info("string_converter: %s: size=%d\n", __func__, s_msg_size);
	sstat.numb_wr_call++;

	return s_msg_size;
}

static ssize_t sshow_stat(struct class *class, struct class_attribute *attr,
	char *buf)
{
	size_t size_stat = sprintf(buf, "Sys statistics:\n"
			"Read calls: %d\n"
			"Write calls: %d\n"
			"Numb convert to upper: %d\n",
			sstat.numb_rd_call, sstat.numb_wr_call,
			sstat.numb_conv);

	pr_info("string_converter: %s: size=%d\n", __func__, size_stat);

	return size_stat;
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

static const struct class_attribute class_attr_str = {
	.attr = { .name = SYS_FILE, .mode = 0666 },
	.show = sshow,
	.store = sstore,
};

static const struct class_attribute class_attr_stat = {
	.attr = { .name = SYS_STAT, .mode = 0444 },
	.show = sshow_stat,
};

static struct proc_dir_entry *pdir;
static struct proc_dir_entry *pfile, *stfile;

static struct class *attr_class;

static int __init conv_init(void)
{
	int ret;

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

	attr_class = class_create(THIS_MODULE, "string_converter");
	if (attr_class == NULL) {
		proc_remove(pdir);
		pr_err("string_converter: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_str);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		proc_remove(pdir);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		proc_remove(pdir);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("string_converter: loaded\n");
	return 0;
}

static void __exit conv_exit(void)
{
	proc_remove(pdir);
	class_destroy(attr_class);
	pr_info("string_converter: closed");
}

module_init(conv_init);
module_exit(conv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Happybolt <dmytro.topikha@gmail.com>");
MODULE_DESCRIPTION("String convert for linux kernel");
MODULE_VERSION("0.1");
