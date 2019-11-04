// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define DIR_NAME	"uc_conv_driver"
#define CONVETER_NAME	"uc_converter"
#define NCALLS_NAME	"num_calls"
#define NPROCESSED_CH_NAME	"num_processed"
#define NCONVERTED_CH_NAME	"num_converted"

static char msg[PAGE_SIZE];
static char buffer[PAGE_SIZE] = "0\n";
static char proc_buffer[PAGE_SIZE] = "0\n";
static char conv_buffer[PAGE_SIZE] = "0\n";
static ssize_t msg_size, buf_size, proc_buffer_size, conv_buffer_size;
static struct proc_dir_entry *uc_dir;
static struct proc_dir_entry *ent_conv, *ent_stat_calls,
			*ent_stat_proc, *ent_stat_conv;

static int total_calls;
static int total_procedded_ch;
static int total_converted_ch;

static char to_upper(const char ch)
{
	total_procedded_ch++;
	proc_buffer_size = sprintf(proc_buffer,
		"Number of processed characters: %d\n",	total_procedded_ch);

	if (ch >= 97 && ch <= 122) {
		total_converted_ch++;
		conv_buffer_size = sprintf(conv_buffer,
		"Number of converted characters: %d\n",	total_converted_ch);
		return (ch - 32);
	}
	return ch;
}

static int to_uppercase(char *input_str, size_t count)
{
	int i;

	if (count > strlen(input_str)) {
		return -1;
	}
	for (i = 0; i < count; i++) {
		input_str[i] = to_upper(input_str[i]);
	}
	return 0;

}

static ssize_t uc_stat_calls_read(struct file *file, char __user *pbuf,
					size_t count, loff_t *ppos)
{

	ssize_t num, not_copied;

	pr_info("uppercase_conv_module: Number of calls: %d\n", total_calls);
	num = min_t(ssize_t, buf_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, buffer, num);
		num -= not_copied;
	}

	buf_size = 0; /* Indicate EOF on next read */

	return num;
}

static ssize_t uc_stat_proc_read(struct file *file, char __user *pbuf,
					size_t count, loff_t *ppos)
{

	ssize_t num, not_copied;

	pr_info("uppercase_conv_module: Number of processed charaters: %d\n",
							total_procedded_ch);
	num = min_t(ssize_t, proc_buffer_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, proc_buffer, num);
		num -= not_copied;
	}

	proc_buffer_size = 0; /* Indicate EOF on next read */

	return num;
}

static ssize_t uc_stat_conv_read(struct file *file, char __user *pbuf,
					size_t count, loff_t *ppos)
{

	ssize_t num, not_copied;

	pr_info("uppercase_conv_module: Number of converted charaters: %d\n",
							total_converted_ch);
	num = min_t(ssize_t, conv_buffer_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, conv_buffer, num);
		num -= not_copied;
	}

	conv_buffer_size = 0; /* Indicate EOF on next read */

	return num;
}

static ssize_t uc_conv_write(struct file *file, const char __user *pbuf,
						size_t count, loff_t *ppos)
{
	ssize_t not_copied;

	pr_info("uppercase_conv_module: Input message: %s\n", msg);

	not_copied = copy_from_user(msg, pbuf, count);

	msg_size = count - not_copied;

	if (to_uppercase(msg, msg_size) < 0) {
		pr_info("uppercase_conv_module: Some error");
	}

	pr_info("uppercase_conv_module: Output message: %s\n", msg);
	total_calls++;
	buf_size = sprintf(buffer, "Number of calls: %d\n", total_calls);
	return msg_size;
}

static ssize_t uc_conv_read(struct file *file, char __user *pbuf,
					size_t count, loff_t *ppos)
{

	ssize_t num, not_copied;

	num = min_t(ssize_t, msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, msg, num);
		num -= not_copied;
	}

	msg_size = 0; /* Indicate EOF on next read */

	return num;
}

static struct file_operations uc_stat_calls = {
	.owner = THIS_MODULE,
	.read = uc_stat_calls_read,
};

static struct file_operations uc_stat_proc = {
	.owner = THIS_MODULE,
	.read = uc_stat_proc_read,
};

static struct file_operations uc_stat_conv = {
	.owner = THIS_MODULE,
	.read = uc_stat_conv_read,
};

static struct file_operations uc_ops = {
	.owner = THIS_MODULE,
	.read = uc_conv_read,
	.write = uc_conv_write,
};

static int uppercase_converter_module_init(void)
{

	pr_info("uppercase_conv_module: module loading\n");
	uc_dir = proc_mkdir(DIR_NAME, NULL);
	if (uc_dir == NULL) {
		pr_err("uppercase_conv_module: error creating procfs directory\n");
		return -ENOMEM;
	}

	ent_conv = proc_create(CONVETER_NAME, 0666, uc_dir, &uc_ops);
	if (ent_conv == NULL) {
		pr_err("uppercase_conv_module: error creating procfs entry 1\n");
		proc_remove(uc_dir);
		return -ENOMEM;
	}

	ent_stat_calls = proc_create(NCALLS_NAME, 0444, uc_dir, &uc_stat_calls);
	if (ent_stat_calls == NULL) {
		pr_err("uppercase_conv_module: error creating procfs entry 1\n");
		proc_remove(uc_dir);
		return -ENOMEM;
	}

	ent_stat_proc = proc_create(NPROCESSED_CH_NAME, 0444,
					uc_dir, &uc_stat_proc);
	if (ent_stat_proc == NULL) {
		pr_err("uppercase_conv_module: error creating procfs entry 1\n");
		proc_remove(uc_dir);
		return -ENOMEM;
	}

	ent_stat_conv = proc_create(NCONVERTED_CH_NAME, 0444,
					uc_dir, &uc_stat_conv);
	if (ent_stat_conv == NULL) {
		pr_err("uppercase_conv_module: error creating procfs entry 1\n");
		proc_remove(uc_dir);
		return -ENOMEM;
	}
	buf_size = proc_buffer_size = conv_buffer_size = 3;
	pr_info("uppercase_conv_module: module loaded\n");

	return 0;
}

static void uppercase_converter_module_exit(void)
{
	proc_remove(uc_dir);
	pr_info("uppercase_conv_module: module terminated");
}

module_init(uppercase_converter_module_init);
module_exit(uppercase_converter_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Uppercase converter, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
