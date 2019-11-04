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
#include <linux/device.h>

#define DIR_NAME_PROCFS "module_05"
#define ENT_NAME_PROCFS "uppercase"
#define CLASS_ENT_NAME_SYSFS "lowercase"
#define CLASS_NAME_SYSFS DIR_NAME_PROCFS

#define STATISTICS_NAME "statistics"

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent_uppercase;

static char str[PAGE_SIZE];
static ssize_t str_size;

static char str_show[PAGE_SIZE];

static struct class *attr_class;

static int count_convert_char_to_up;
static int count_convert_char_to_down;


void make_stat(char *buf)
{
	sprintf(buf,
		"converted lowercase characters:\t%d\n"
		"converted uppercase characters:\t%d\n"
		"converted characters:\t%d\n",
		count_convert_char_to_up,
		count_convert_char_to_down,
		count_convert_char_to_up + count_convert_char_to_down);
}

void lowercase(void)
{
	int i;

	for (i = 0; i < str_size; ++i) {
		str_show[i] = str[i];
		if (str_show[i] >= 'a' && str_show[i] <= 'z') {
			str_show[i] -= 'a' - 'A';
			count_convert_char_to_up++;
		}
	}
}

static ssize_t stat_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	pr_info("%s: str_show = %s\n", __func__, str_show);

	make_stat(buf);
	return strlen(buf);
}


static ssize_t rw_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	pr_info("%s: str_show = %s\n", __func__, str_show);

	sprintf(buf, "%s", str_show);
	return strlen(buf);
}

static ssize_t rw_store(struct class *class,
	struct class_attribute *attr, const char *buf, size_t count)
{
	memcpy(str, buf, count);

	str_size = count;

	lowercase();

	pr_info("%s: str = %s\n", __func__, str);
	return count;
}

/*CLASS_ATTR_RW(rw);*/
struct class_attribute class_attr_rw = {
	.attr = { .name = CLASS_ENT_NAME_SYSFS, .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

struct class_attribute class_attr_statistics = {
	.attr = { .name = STATISTICS_NAME, .mode = 0444 },
	.show	= stat_show
};

int init_sysfs(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME_SYSFS);
	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", __func__);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n", __func__);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_statistics);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n", __func__);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("%s: module loaded\n", __func__);

	return 0;
}

void uppercase(void)
{
	int i;

	for (i = 0; i < str_size; ++i) {
		str_show[i] = str[i];
		if (str_show[i] >= 'A' && str_show[i] <= 'Z') {
			str_show[i] += 'a' - 'A';
			count_convert_char_to_down++;
		}
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

	uppercase();

	pr_info("%s: msg_size=%d\n", __func__, str_size);

	return str_size;
}

static ssize_t uppercase_read(struct file *file,
	char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;

	pr_info("%s: count=%d\n", __func__, count);

	num = min_t(ssize_t, str_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, str_show, num);
		num -= not_copied;
	}

	str_size = 0; /* Indicate EOF on next read */

	pr_info("%s: return=%d\n", __func__,  num);
	return num;
}

static ssize_t stat_read(struct file *file,
	char __user *pbuf, size_t count, loff_t *ppos)
{
	static char buf[PAGE_SIZE];
	static ssize_t count_read;
	ssize_t not_copied;

	make_stat(buf);
	count_read++;
	if (count_read <= 1) {
		not_copied =  copy_to_user(pbuf, buf, strlen(buf));
		count_read++;
		return strlen(buf) - not_copied;
	} else if (count_read >= 4) { // for utils cat
		count_read = 0;
	}

	return 0;
}

static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.read = stat_read
};

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

	ent_uppercase = proc_create(STATISTICS_NAME, 0444, dir, &stat_ops);
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

	if (ret < 0)
		return ret;

	ret = init_sysfs();

	if (ret < 0)
		proc_remove(dir);

	return ret;
}

static void module_05_exit(void)
{
	pr_info("%s: module exit\n",  __func__);
	proc_remove(dir);

	class_destroy(attr_class);
}


module_init(module_05);
module_exit(module_05_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("A simple Kernel module.");
MODULE_VERSION("0.1");
