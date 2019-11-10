// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

unsigned long prev_jiffies;

static ssize_t rel_time_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{
	unsigned long cur_jiffies;
	unsigned long diff_jiffies;
	cur_jiffies = jiffies;

	diff_jiffies = cur_jiffies - prev_jiffies;
	pr_info("timing_module: Previous rel tim: %lu\n", prev_jiffies);
	pr_info("timing_module: Curent rel time: %lu\n", cur_jiffies);
	pr_info("timing_module: Diff rel time: %lu\n", diff_jiffies);
	prev_jiffies = cur_jiffies;
	sprintf(buf, "Diff rel time: %lu\n", diff_jiffies);
	return strlen(buf);
}

struct class_attribute class_attr_rel_time = {
	.attr = { .name = "rel_time", .mode = 0444 },
	.show	= rel_time_show,
};

static struct class *attr_class = 0;

static int timing_module_init(void)
{
	int ret;

	pr_info("timing_module: module loading\n");

	attr_class = class_create(THIS_MODULE, "class_timing");
	if (attr_class == NULL) {
		pr_err("timing_module: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_rel_time);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("timing_module: module loaded\n");
	return 0;
}

static void timing_module_exit(void)
{
	class_remove_file(attr_class, &class_attr_rel_time);
	class_destroy(attr_class);

	pr_info("timing_module: module exited\n");
}

module_init(timing_module_init);
module_exit(timing_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Timing managment, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
