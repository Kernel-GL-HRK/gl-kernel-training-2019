#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/err.h>



static int amount_read_calls, amount_write_calls, amount_converted_chars,amount_chars_finished_converted;
static ssize_t text_for_convert_size;
static char text_for_convert[PAGE_SIZE];


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

static ssize_t option_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	pr_info("sysfs_module_uppercase: write data");

	size_t template_count;
	if (count <= PAGE_SIZE)
		template_count = count;
	else
		template_count = PAGE_SIZE - 1;
	int i;
	for(i = 0; i < template_count; i++)
		text_for_convert[i] = buf[i];
	text_for_convert[template_count] = 0;


	amount_write_calls++;
	convert_to_upper_case(text_for_convert);
	return count;
}

static ssize_t option_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("sysfs_module_uppercase: read data");
	
	strcpy(buf, text_for_convert);

	amount_read_calls++;

	return strlen(buf);
}

static ssize_t statistic_show(struct class *class, struct class_attribute *attr, char *buf)
{
	char buff[600] = "";
	/*strcat(buff, "View statistic \n");
	sprintf(buff, "amount write calls %d \n", amount_write_calls);
	sprintf(buff, "amount calls %d \n", amount_read_calls);
	sprintf(buff, "amount converted chars %d \n", amount_converted_chars);
	sprintf(buff, "amount chars finished converted %d \n", amount_chars_finished_converted);*/

	sprintf(buff, "View statistic \n"
		"amount write calls %d \n"
		"amount calls %d \n"
		"amount converted chars %d \n"
		"amount chars finished converted %d \n"
	 , amount_write_calls,amount_read_calls,
	  amount_converted_chars,amount_chars_finished_converted);
	strcpy(buf, buff);
	return strlen(buf);
}

struct class_attribute class_attr_statisitc = {
	.attr = { .name = "statistic", .mode = 0444 },
	.show	= statistic_show,

};

static struct class *attr_class = 0;

struct class_attribute class_attr_option = {
	.attr = { .name = "option", .mode = 0666 },
	.show	= option_show,
	.store	= option_store
};

static struct proc_dir_entry *entstat;

//init moudle
static int mymodule_init(void)
{
	static int amount_read_calls = 0;
	static int amount_write_calls = 0;
	static int amount_converted_chars = 0;
	static int amount_chars_finished_converted = 0;

	int ret;

	attr_class = class_create(THIS_MODULE, "base_class");
	if (attr_class == NULL) {
		pr_err("procfs_module_uppercase: error creating sysfs entry\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_option);
	if (ret) {
		pr_err("sysfs_module_uppercase: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_statisitc);
	if (ret) {
		pr_err("sysfs_module_uppercase: error creating sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_option);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("sysfs_module_uppercase: module loaded\n");
	return 0;
}

//exit module
static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_statisitc);
	class_remove_file(attr_class, &class_attr_option);
	class_destroy(attr_class);
	pr_info("procfs_module_uppercase: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Vladyslav");
MODULE_DESCRIPTION(" Use procfs kernel module for uppercase.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
