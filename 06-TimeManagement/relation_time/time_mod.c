/*
 * time_mod.c  Relation time kernel module
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

unsigned long prevJiffiesVal;

static ssize_t relattime_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	pr_info("mymodule: read data");

	unsigned long diff = jiffies - prevJiffiesVal;

	prevJiffiesVal = jiffies;

	sprintf(buf, "time passed since previous read: %u.%03u seconds\n",
		diff/1000, diff%1000);

	return strlen(buf);
}

struct class_attribute class_attr_relattime = {
	.attr = { .name = "relattime", .mode = 0666 },
	.show	= relattime_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_relattime);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	prevJiffiesVal = jiffies;

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_relattime);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("Relation time kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
