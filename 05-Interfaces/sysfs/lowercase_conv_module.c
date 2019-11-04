// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

static char msg[PAGE_SIZE];

static int total_calls;

static char ch_to_lower(const char ch)
{

	if (ch >= 'A' && ch <= 'Z') {
		return (ch + 32);
	}
	return ch;
}

static int to_lowercase(char *input_str, size_t count)
{
	int i;

	if (count > strlen(input_str)) {
		return -1;
	}
	for (i = 0; i < count; i++) {
		input_str[i] = ch_to_lower(input_str[i]);
	}
	return 0;

}

static ssize_t calls_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{
	pr_info("lowercase_conv_module: num of calls is %d\n", total_calls);
	sprintf(buf, "num of calls: %d\n", total_calls);
	return strlen(buf);
}

static ssize_t conv_string_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{
	pr_info("lowercase_conv_module: output: %s\n", msg);
	strcpy(buf, msg);
	return strlen(buf);
}

static ssize_t conv_string_store(struct class *class,
					struct class_attribute *attr,
					const char *buf, size_t count)
{
	total_calls++;
	pr_info("lowercase_conv_module: input: %s\n", buf);
	strncpy(msg, buf, strlen(buf));
	if (to_lowercase(msg, count) < 0) {
		pr_info("lowercase_conv_module: Some error");
	}

	return count;
}

/*CLASS_ATTR_RW(conv_string);*/
struct class_attribute class_attr_conv_string = {
	.attr = { .name = "conv_string", .mode = 0666 },
	.show	= conv_string_show,
	.store	= conv_string_store
};

struct class_attribute class_attr_stat_calls = {
	.attr = { .name = "num_calls", .mode = 0444 },
	.show	= calls_show,
};

static struct class *attr_class = 0;

static int lowercase_converter_module_init(void)
{
	int ret;

	pr_info("lowercase_conv_module: module loading\n");

	attr_class = class_create(THIS_MODULE, "class_conv_string");
	if (attr_class == NULL) {
		pr_err("lowercase_conv_module: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_conv_string);
	if (ret) {
		pr_err("lowercase_conv_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat_calls);
	if (ret) {
		pr_err("lowercase_conv_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("lowercase_conv_module: module loaded\n");
	return 0;
}

static void lowercase_converter_module_exit(void)
{
	class_remove_file(attr_class, &class_attr_conv_string);
	class_destroy(attr_class);

	pr_info("lowercase_conv_module: module exited\n");
}

module_init(lowercase_converter_module_init);
module_exit(lowercase_converter_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Lowercase converter, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
