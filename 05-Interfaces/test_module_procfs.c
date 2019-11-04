#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DIR_NAME	"test_module"
#define FILE_NAME	"string_convert"
#define TOTAL_STAT_NAME	"total_stat"

static struct proc_dir_entry *dir_test;
static struct proc_dir_entry *file_test;
static struct proc_dir_entry *total_stat_test;

#define DBG_INFO(x, arg...) \
pr_info("Test_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("Test_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

struct test_module_data {
	char *buffer;
	size_t size_buffer;
	size_t total_calls;
	size_t total_chars;
	size_t conv_chars;
};

static struct test_module_data data = {
	.buffer = NULL,
	.size_buffer = 0,
	.total_calls = 0,
	.total_chars = 0,
	.conv_chars = 0,
};

size_t to_uppercase(char *pbuf, size_t count)
{
	size_t conv_cnt, i;

	conv_cnt = 0;

	for (i = 1; i < count; i++) {
		if ((*pbuf >= 'a') && (*pbuf <= 'z')) {
			(*pbuf) -= 32;
			conv_cnt++;
		}
		pbuf++;
	}
	return conv_cnt;
}

static int test_open(struct inode *node, struct file *f)
{
	f->private_data = &data;
	data.total_calls++;

	DBG_INFO("File is opened\n");
	return 0;
}

static int test_release(struct inode *node, struct file *f)
{
	DBG_INFO("File is closed\n");
	return 0;
}

static ssize_t test_read(struct file *file, char __user *pbuf,
						size_t count, loff_t *ppos)
{
	struct test_module_data *data = file->private_data;
	ssize_t nbytes;

	nbytes = data->size_buffer;

	if (*ppos >= PAGE_SIZE) {
		return -ENOMEM;
	}
	if ((*ppos + data->size_buffer) >= PAGE_SIZE) {
		nbytes = PAGE_SIZE - *ppos;
	}

	copy_to_user(pbuf, data->buffer + *ppos, nbytes);

	DBG_INFO("Read %d\n", nbytes);

	*ppos += nbytes;
	data->size_buffer = 0;

	return nbytes;
}

static ssize_t test_stat_read(struct file *file, \
						char __user *pbuf, \
						size_t count, loff_t *ppos)
{
	char stat_buffer[256] = {0};
	ssize_t nbytes;

	sprintf(stat_buffer, "TOTAL_CONV = %i\nTOTAL_CHARS = %i\nTOTAL_CALLS = %i\n",\
			data.conv_chars, data.total_chars, data.total_calls);

	nbytes = strlen(stat_buffer) + 1;
	DBG_INFO("Read %d\n", nbytes);

		if (*ppos >= nbytes) {
			return 0;
		}

	copy_to_user(pbuf, stat_buffer + *ppos, nbytes);
	*ppos += nbytes;

	return nbytes;
}

static ssize_t test_write(struct file *file, const char __user *pbuf, \
						size_t count, loff_t *ppos)
{
	struct test_module_data *data = file->private_data;
	ssize_t nbytes;

	nbytes = count;

	if (*ppos >= PAGE_SIZE) {
		return -EFAULT;
	}
	if ((*ppos + count) >= PAGE_SIZE) {
		nbytes = PAGE_SIZE - *ppos;
	}

	DBG_INFO("Write %d\n", nbytes);

	copy_from_user(data->buffer + *ppos, pbuf, nbytes);
	data->conv_chars = to_uppercase(data->buffer + *ppos, nbytes);
	data->total_chars = nbytes - 1;
	data->size_buffer = nbytes;
	*ppos += nbytes;

	return nbytes;
}

static const struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.open = test_open,
	.read = test_read,
	.write = test_write,
	.release = test_release,
};

static const struct file_operations file_stat_ops = {
	.owner = THIS_MODULE,
	.read = test_stat_read,
};

static int __init test_module_init(void)
{
	dir_test = proc_mkdir(DIR_NAME, NULL);
		if (dir_test == NULL) {
			DBG_ERROR("Error while creating procfs directory\n");
		return -ENOMEM;
	}

	file_test = proc_create(FILE_NAME, 0666, dir_test, &file_ops);
		if (file_test == NULL) {
			DBG_ERROR("Error while creating procfs entry\n");
			remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	total_stat_test = proc_create(TOTAL_STAT_NAME, 0444, \
								dir_test, \
								&file_stat_ops);
		if (total_stat_test == NULL) {
			DBG_ERROR("Error while creating procfs entry\n");
			remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	char *buffer;

	buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	data.buffer = buffer;

	DBG_INFO("Module is loaded\n");
	return 0;
}

static void __exit test_module_stop(void)
{
	remove_proc_entry(FILE_NAME, dir_test);
	remove_proc_entry(TOTAL_STAT_NAME, dir_test);
	proc_remove(dir_test);

		if (data.buffer) {
			kfree(data.buffer);
		}

	DBG_INFO("Module is unloaded\n");
}

module_init(test_module_init);
module_exit(test_module_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("Test module for linux kernel");
MODULE_VERSION("0.1");
