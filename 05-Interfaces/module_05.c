// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  module_05.c - Test module convert string
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define DIR_NAME_PROCFS "module_05"
#define ENT_NAME_PROCFS "uppercase"

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent_uppercase;

static char str[PAGE_SIZE];
static ssize_t str_size;

static char str_show[PAGE_SIZE];


void uppercase(void)
{
	int i;

	for (i = 0; i < str_size; ++i) {
		str_show[i] = str[i];
		if (str_show[i] >= 'A' && str_show[i] <= 'Z')
			str_show[i] += 'a' - 'A';
	}
}

static ssize_t uppercase_write(struct file *file,
	const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	static char buf[PAGE_SIZE];
	char *path;

	pr_info("%s: read file path = %s\n",
		__func__, file->f_path.dentry->d_iname);

	path = dentry_path_raw(file->f_path.dentry, buf, sizeof(buf));

	pr_info("%s: read file full path = %s\n", __func__, path);

	pr_info("%s: count=%d\n", __func__, count);

	not_copied = copy_from_user(str, pbuf, count);

	str_size = count - not_copied;

	pr_info("%s: msg_size=%d\n", __func__, str_size);

	return str_size;
}

static ssize_t uppercase_read(struct file *file,
	char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;

	pr_info("%s: count=%d\n", __func__, count);

	uppercase();

	num = min_t(ssize_t, str_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, str_show, num);
		num -= not_copied;
	}

	str_size = 0; /* Indicate EOF on next read */

	pr_info("%s: return=%d\n", __func__,  num);
	return num;
}



static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = uppercase_read,
	.write = uppercase_write,
};

int init_procfs(void)
{
	dir = proc_mkdir(DIR_NAME_PROCFS, NULL);
	if (dir == NULL) {
		pr_err("%s: error creating procfs directory\n", __func__);
		return -ENOMEM;
	}

	ent_uppercase = proc_create(ENT_NAME_PROCFS, 0666, dir, &myops);
	if (ent_uppercase == NULL) {
		pr_err("%s: error creating procfs entry\n", __func__);
		remove_proc_entry(DIR_NAME_PROCFS, NULL);
		return -ENOMEM;
	}
	return 0;
}



static int module_05(void)
{
	int ret;

	pr_info("%s: module starting\n",  __func__);
	ret = init_procfs();

	return ret;
}

static void module_05_exit(void)
{
	pr_info("%s: module exit\n",  __func__);
	proc_remove(dir);
}


module_init(module_05);
module_exit(module_05_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("A simple Kernel module.");
MODULE_VERSION("0.1");
