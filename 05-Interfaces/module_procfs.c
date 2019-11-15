// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>


#define DIR_NAME	"my_module"
#define FILE_NAME	"string_convertor"
#define STAT_NAME	"statistic"
#define CHAR_SHIFT	32

static char msg[PAGE_SIZE];
static ssize_t msg_size;

static int ReadCalls;
static int CharsProcessed;
static int CharsConverted;

static struct proc_dir_entry *dir_name;
static struct proc_dir_entry *file_name;
static struct proc_dir_entry *stat_name;

#define DBG_INFO(x, arg...) \
pr_info("My_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("My_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)



size_t uppercase(char *pbuf, size_t count)
{
	size_t cnt, i;

	cnt = 0;

	for (i = 1; i < count; i++) {
		CharsProcessed++;
		if ((*pbuf >= 'a') && (*pbuf <= 'z')) {
			(*pbuf) -= CHAR_SHIFT;
			cnt++;
		}
		pbuf++;
	}
	return cnt;
}


static ssize_t my_read(struct file *file, char __user *pbuf,
						size_t count, loff_t *ppos)
{
	ssize_t ret;

	ret = simple_read_from_buffer(pbuf, count, ppos, msg, msg_size);

	DBG_INFO("Read %d\n", ret);
	ReadCalls++;

	return ret;
}

static ssize_t my_stat_read(struct file *file, char __user *pbuf,
						size_t count, loff_t *ppos)
{

	ssize_t ret = 0;

	char buff[256] = "";
	size_t buff_size;

	sprintf(buff, "read calls = %i\nchars processed = %i\nchars converted = %i\n",
			ReadCalls, CharsProcessed, CharsConverted);

	buff_size = strlen(buff) + 1;

	ret = simple_read_from_buffer(pbuf, count, ppos, buff, buff_size);
	DBG_INFO("Read %d\n", ret);

	return ret;
}

static ssize_t my_write(struct file *file, const char __user *pbuf,
						size_t count, loff_t *ppos)
{
	ssize_t ret;

	ret = simple_write_to_buffer(msg, sizeof(msg), ppos, pbuf, count);

	DBG_INFO("Write %d\n", ret);

	msg_size = ret;


	CharsConverted = uppercase(msg, msg_size);

	return ret;
}

static const struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

static const struct file_operations file_stat_ops = {
	.owner = THIS_MODULE,
	.read = my_stat_read,
};

static int __init my_module_init(void)
{
	dir_name = proc_mkdir(DIR_NAME, NULL);
		if (dir_name == NULL) {
			DBG_ERROR("Error creating procfs directory\n");
		return -ENOMEM;
	}

	file_name = proc_create(FILE_NAME, 0666, dir_name, &file_ops);
		if (file_name == NULL) {
			DBG_ERROR("Error creating procfs entry\n");
			remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	stat_name = proc_create(STAT_NAME, 0444, dir_name, &file_stat_ops);
		if (stat_name == NULL) {
			DBG_ERROR("Error creating procfs entry\n");
			remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}
	ReadCalls = 0;
	CharsProcessed = 0;
	CharsConverted = 0;
	DBG_INFO("Module is loaded\n");
	return 0;
}

static void __exit my_module_exit(void)
{
	remove_proc_entry(FILE_NAME, dir_name);
	remove_proc_entry(STAT_NAME, dir_name);
	proc_remove(dir_name);

	DBG_INFO("Module is unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);


MODULE_AUTHOR("Lapin Vadim <lapin.vadim@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");