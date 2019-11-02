// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static char msg[PAGE_SIZE];
static ssize_t msg_size;
static struct proc_dir_entry *ent;

static char to_upper(const char ch)
{
	if (ch >= 97 && ch <= 122) {
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

static struct file_operations uc_ops = {
	.owner = THIS_MODULE,
	.read = uc_conv_read,
	.write = uc_conv_write,
};

static int uppercase_converter_module_init(void)
{
	pr_info("uppercase_conv_module: module loading\n");
	ent = proc_create("uc_conv_procfs", 0666, NULL, &uc_ops);
	pr_info("uppercase_conv_module: module loaded\n");
	return 0;
}

static void uppercase_converter_module_exit(void)
{
	proc_remove(ent);
	pr_info("uppercase_conv_module: module terminated");
}

module_init(uppercase_converter_module_init);
module_exit(uppercase_converter_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Uppercase convertor, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
