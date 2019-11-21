// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/time.h>

#define NVALUE 31
#define NSTAT 10
#define NANO 1000000000

static long diff_calc(const struct timespec t0,
				const struct timespec t1)
{

	struct timespec ts_delta;

	ts_delta = timespec_sub(t1, t0);
	return ts_delta.tv_sec * NANO + ts_delta.tv_nsec;
}

static void allocation(int type, int ind)
{
	int j;
	size_t size;
	void *ptr;
	long diff;
	struct timespec t0, t1, tf0, tf1;
	unsigned long page;
	unsigned long avtm = 0;
	unsigned long avtf = 0;
	long min_tm = LONG_MAX;
	long max_tm = 0;
	long min_tf = LONG_MAX;
	long max_tf = 0;

	size = 1ull << ind;

	for (j = 0; j < NSTAT; j++) {
		ptr = NULL;
		if (type == 1) {
			getnstimeofday(&t0);
			ptr = kzalloc(size, GFP_KERNEL);
			getnstimeofday(&t1);
		} else if (type == 2) {
			getnstimeofday(&t0);
			ptr = vmalloc(size);
			getnstimeofday(&t1);
		} else if (type == 3) {
			getnstimeofday(&t0);
			page = __get_free_pages(GFP_KERNEL, ind);
			getnstimeofday(&t1);
		} else {// (type == 0) and default
			getnstimeofday(&t0);
			ptr = kmalloc(size, GFP_KERNEL);
			getnstimeofday(&t1);
		}
		diff = diff_calc(t0, t1);
		if (diff < min_tm)
			min_tm = diff;
		if (diff > max_tm)
			max_tm = diff;
		avtm += diff;

		if ((type == 3) || (ptr)) {
			if (type == 3) {
				getnstimeofday(&tf0);
				free_pages(page, ind);
				getnstimeofday(&tf1);
			} else if (type == 2) {
				getnstimeofday(&tf0);
				vfree(ptr);
				getnstimeofday(&tf1);
			} else {
				getnstimeofday(&tf0);
				kfree(ptr);
				getnstimeofday(&tf1);
			}
			diff = diff_calc(tf0, tf1);
			if (diff < min_tf)
				min_tf = diff;
			if (diff > max_tf)
				max_tf = diff;
			avtf += diff;
		} else {
			pr_info("%2d %22d Memory not allocated\n", ind, size);
			return;
		}
	}
	avtm = avtm/NSTAT;
	avtf = avtf/NSTAT;
	pr_info("%2d %15d %12ld %12lu %12ld %12ld %12lu %12ld\n",
	ind, size, min_tm, avtm, max_tm, min_tf, avtf, max_tf);

}

static void get_alloc_stat(int type)
{
	int ind;

	for (ind = 0; ind < NVALUE; ind++)
		allocation(type, ind);
}

static ssize_t kmalloc_stat_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{

	get_alloc_stat(0);
	return strlen(buf);
}

static ssize_t kzalloc_stat_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{
	get_alloc_stat(1);
	return strlen(buf);
}

static ssize_t vmalloc_stat_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{

	get_alloc_stat(2);
	return strlen(buf);
}

static ssize_t get_pages_stat_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{
	get_alloc_stat(3);
	return strlen(buf);
}

struct class_attribute class_attr_kmalloc_stat = {
	.attr = { .name = "kmalloc_stat", .mode = 0444 },
	.show	= kmalloc_stat_show,
};

struct class_attribute class_attr_kzalloc_stat = {
	.attr = { .name = "kzalloc_stat", .mode = 0444 },
	.show	= kzalloc_stat_show,
};

struct class_attribute class_attr_vmalloc_stat = {
	.attr = { .name = "vmalloc_stat", .mode = 0444 },
	.show	= vmalloc_stat_show,
};

struct class_attribute class_attr_get_pages_stat = {
	.attr = { .name = "get_pages", .mode = 0444 },
	.show	= get_pages_stat_show,
};

static struct class *attr_class;

static int memory_module_init(void)
{
	int ret;

	pr_info("memory_module: module loading\n");

	attr_class = class_create(THIS_MODULE, "class_memory");
	if (attr_class == NULL) {
		pr_err("memory_module: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_kmalloc_stat);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_kzalloc_stat);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_vmalloc_stat);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_get_pages_stat);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("memory_module: module loaded\n");

	return 0;
}

static void memory_module_exit(void)
{
	class_remove_file(attr_class, &class_attr_kmalloc_stat);
	class_remove_file(attr_class, &class_attr_kzalloc_stat);
	class_remove_file(attr_class, &class_attr_vmalloc_stat);
	class_remove_file(attr_class, &class_attr_get_pages_stat);
	class_destroy(attr_class);

	pr_info("memory_module: module exited\n");
}

module_init(memory_module_init);
module_exit(memory_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Memory managment, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
