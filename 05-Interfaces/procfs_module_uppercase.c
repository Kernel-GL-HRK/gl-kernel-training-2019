#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>


static int amount_read_calls, amount_write_calls, amount_converted_chars,amount_chars_finished_converted;
static char text_for_convert[PAGE_SIZE];
static ssize_t text_for_convert_size;




static ssize_t read_option(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t result;

	result = simple_read_from_buffer(pbuf, count, ppos, text_for_convert, text_for_convert_size);
	pr_info("procfs_module_uppercase: read option size %d bytes\n", result);

	amount_read_calls++;

	return result;
}

static void print_statistic(char *pbuf, char *template, int value)
{
	char one_line[45] = "                          ->        \n";
	int i = 0;
	while(template[i] != 0)
	{
		one_line[2 + i] = template[i];
		i++;
	}
	i = 0;
	do
	{
		one_line[35 - i] = '0' + value % 10;
		value /= 10;
		i++;
	} while (value != 0);
	strcat(pbuf, one_line);
}

static void convert_to_upper_case(char *temp_buffer)
{
	int i = 0;
	while(temp_buffer[i] != 0)
	{
		if(temp_buffer[i] >= 'a' && temp_buffer[i] <= 'z')
		{
			temp_buffer[i] = temp_buffer[i] - 'a' + 'A';
			amount_chars_finished_converted++;
		}
		i++;
		amount_converted_chars++;
	}
}

static ssize_t write_option(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t result;

	result = simple_write_to_buffer(text_for_convert, sizeof(text_for_convert), ppos, pbuf, count);
	pr_info("procfs_module_uppercase: write to parameter %d bytes \n", result);
	text_for_convert_size = result;

	amount_converted_chars = 0;
	amount_chars_finished_converted = 0;

	convert_to_upper_case(text_for_convert);

	amount_write_calls++;

	return result;
}


static ssize_t print_total_statistic(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t result = 0;

	char buff[610] = "";
	size_t buff_size;

	strcat(buff, " statistic \n");
	print_statistic(buff, "amount write calls", amount_write_calls);
	print_statistic(buff, "amount calls", amount_read_calls);
	print_statistic(buff, "amount converted chars", amount_converted_chars);
	print_statistic(buff, "amount chars finished converted", amount_chars_finished_converted);
	buff_size = strlen(buff) + 1;
	result = simple_read_from_buffer(pbuf, count, ppos, buff, buff_size);
	pr_info("procfs_module_uppercase: read option size %d bytes\n", result);
	return result;
}

static struct file_operations for_convert =
{
	.owner = THIS_MODULE,
	.read = read_option,
	.write = write_option,
};

static struct proc_dir_entry *ent;

static struct file_operations function_for_staticstic =
{
	.owner = THIS_MODULE,
	.read = print_total_statistic,
};

static struct proc_dir_entry *entstat;

//init moudle
static int mymodule_init(void)
{
	static int amount_read_calls = 0;
	static int amount_write_calls = 0;
	static int amount_converted_chars = 0;
	static int amount_chars_finished_converted = 0;

	ent = proc_create("convert_to_upper_case", 0666, NULL, &for_convert);
	if (ent == NULL) {
		pr_err("procfs_module_uppercase: error creating procfs entry\n");
		return -ENOMEM;
	}

	entstat = proc_create("view_statistic", 0666, NULL, &function_for_staticstic);
	if(entstat == NULL) {
		proc_remove(ent);
		pr_err("procfs_module_uppercase: error creating procfs entry stat\n");
		return -ENOMEM;
	}

	pr_info("procfs_module_uppercase: module loaded\n");
	return 0;
}

//exit module
static void mymodule_exit(void)
{
	proc_remove(ent);
	proc_remove(entstat);
	pr_info("procfs_module_uppercase: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Vladyslav");
MODULE_DESCRIPTION(" Use procfs kernel module for uppercase.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
