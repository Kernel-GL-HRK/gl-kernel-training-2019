/*
 * mod_abs_time.c  Absolute time return module
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/time.h>

static struct timespec prevReadAbsTime;
static struct class *attr_class;

static ssize_t abstime_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	pr_info("timemodule: read data");

	sprintf(buf, "time previous reading: %u.%09u seconds\n",
		prevReadAbsTime.tv_sec, prevReadAbsTime.tv_nsec);
	getnstimeofday(&prevReadAbsTime);

	return strlen(buf);
}

static struct class_attribute class_attr_abstime = {
	.attr = { .name = "abstime", .mode = 0666 },
	.show	= abstime_show,
};

static int timemodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("timemodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_abstime);
	if (ret) {
		pr_err("timemodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	getnstimeofday(&prevReadAbsTime);

	pr_info("timemodule: module loaded\n");
	return 0;
}

static void timemodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_abstime);
	class_destroy(attr_class);

	pr_info("timemodule: module exited\n");
}

module_init(timemodule_init);
module_exit(timemodule_exit);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("A simple absolute time kernel module.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
