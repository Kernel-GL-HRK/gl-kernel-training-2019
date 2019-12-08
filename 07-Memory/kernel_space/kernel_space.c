/*
 * time_mod.c  Memory allocating test kernel module
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/time.h>

#define DBG_INFO(x, arg...) \
pr_info("My_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("My_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

static char *buffer;
static void *pt;

void alloc_tests(void)
{
	struct timespec time_alloc1, time_alloc2, result_alloc;
	struct timespec time_free1, time_free2, result_free;
	int i, shift = 0;
	static int Iter = 18;

	shift += sprintf(buffer+shift, "kmalloc\n");
	shift += sprintf(buffer+shift, "iteration, size(hex), time, time free\n");
	for (i = 0; i <= Iter*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		getnstimeofday(&time_alloc1);
		pt = kmalloc(size, GFP_KERNEL);
		getnstimeofday(&time_alloc2);
		if (pt == NULL) {
			shift += sprintf(buffer+shift, "pt = NULL\n");
			break;
		}
		getnstimeofday(&time_free1);
		kfree(pt);
		getnstimeofday(&time_free2);

		result_alloc.tv_sec = time_alloc2.tv_sec - time_alloc1.tv_sec;
		result_alloc.tv_nsec = time_alloc2.tv_nsec - time_alloc1.tv_nsec;
		if (result_alloc.tv_nsec < 0) {
			result_alloc.tv_nsec += 1000000000;
			result_alloc.tv_sec -= 1;
		}
		result_free.tv_sec = time_free2.tv_sec - time_free1.tv_sec;
		result_free.tv_nsec = time_free2.tv_nsec - time_free1.tv_nsec;
		if (result_free.tv_nsec < 0) {
			result_free.tv_nsec += 1000000000;
			result_free.tv_sec -= 1;
		}
		shift += sprintf(buffer+shift, "%i, 0x%zx, %ld.%09ld, %ld.%09ld\n",
			i/2, size, result_alloc.tv_sec, result_alloc.tv_nsec, result_free.tv_sec, result_free.tv_nsec);
	}

	shift += sprintf(buffer+shift, "kzalloc\n");
	shift += sprintf(buffer+shift, "iteration, size(hex), time, time free\n");
	for (i = 0; i <= Iter*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		getnstimeofday(&time_alloc1);
		pt = kzalloc(size, GFP_KERNEL);
		getnstimeofday(&time_alloc2);
		if (pt == NULL) {
			shift += sprintf(buffer+shift, "pt = NULL\n");
			break;
		}
		getnstimeofday(&time_free1);
		kzfree(pt);
		getnstimeofday(&time_free2);

		result_alloc.tv_sec = time_alloc2.tv_sec - time_alloc1.tv_sec;
		result_alloc.tv_nsec = time_alloc2.tv_nsec - time_alloc1.tv_nsec;
		if (result_alloc.tv_nsec < 0) {
			result_alloc.tv_nsec += 1000000000;
			result_alloc.tv_sec -= 1;
		}
		result_free.tv_sec = time_free2.tv_sec - time_free1.tv_sec;
		result_free.tv_nsec = time_free2.tv_nsec - time_free1.tv_nsec;
		if (result_free.tv_nsec < 0) {
			result_free.tv_nsec += 1000000000;
			result_free.tv_sec -= 1;
		}
		shift += sprintf(buffer+shift, "%i, 0x%zx, %ld.%09ld, %ld.%09ld\n",
			i/2, size, result_alloc.tv_sec, result_alloc.tv_nsec, result_free.tv_sec, result_free.tv_nsec);
	}

	shift += sprintf(buffer+shift, "vmalloc\n");
	shift += sprintf(buffer+shift, "iteration, size(hex), time, time free\n");
	for (i = 0; i <= Iter*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		getnstimeofday(&time_alloc1);
		pt = vmalloc(size);
		getnstimeofday(&time_alloc2);
		if (pt == NULL) {
			shift += sprintf(buffer+shift, "pt = NULL\n");
			break;
		}
		getnstimeofday(&time_free1);
		vfree(pt);
		getnstimeofday(&time_free2);

		result_alloc.tv_sec = time_alloc2.tv_sec - time_alloc1.tv_sec;
		result_alloc.tv_nsec = time_alloc2.tv_nsec - time_alloc1.tv_nsec;
		if (result_alloc.tv_nsec < 0) {
			result_alloc.tv_nsec += 1000000000;
			result_alloc.tv_sec -= 1;
		}
		result_free.tv_sec = time_free2.tv_sec - time_free1.tv_sec;
		result_free.tv_nsec = time_free2.tv_nsec - time_free1.tv_nsec;
		if (result_free.tv_nsec < 0) {
			result_free.tv_nsec += 1000000000;
			result_free.tv_sec -= 1;
		}
		shift += sprintf(buffer+shift, "%i, 0x%zx, %ld.%09ld, %ld.%09ld\n",
			i/2, size, result_alloc.tv_sec, result_alloc.tv_nsec, result_free.tv_sec, result_free.tv_nsec);
	}

	shift += sprintf(buffer+shift, "GFP\n");
	shift += sprintf(buffer+shift, "iteration, size(hex), time, time free\n");
	for (i = 0; i <= Iter*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		getnstimeofday(&time_alloc1);
		pt = (void *)__get_free_pages(GFP_KERNEL, size);
		getnstimeofday(&time_alloc2);
		if (pt == NULL) {
			shift += sprintf(buffer+shift, "pt = NULL\n");
			break;
		}
		getnstimeofday(&time_free1);
		free_pages((unsigned long)pt, size);
		getnstimeofday(&time_free2);

		result_alloc.tv_sec = time_alloc2.tv_sec - time_alloc1.tv_sec;
		result_alloc.tv_nsec = time_alloc2.tv_nsec - time_alloc1.tv_nsec;
		if (result_alloc.tv_nsec < 0) {
			result_alloc.tv_nsec += 1000000000;
			result_alloc.tv_sec -= 1;
		}
		result_free.tv_sec = time_free2.tv_sec - time_free1.tv_sec;
		result_free.tv_nsec = time_free2.tv_nsec - time_free1.tv_nsec;
		if (result_free.tv_nsec < 0) {
			result_free.tv_nsec += 1000000000;
			result_free.tv_sec -= 1;
		}
		shift += sprintf(buffer+shift, "%i, 0x%zx, %ld.%09ld, %ld.%09ld\n",
			i/2, size, result_alloc.tv_sec, result_alloc.tv_nsec, result_free.tv_sec, result_free.tv_nsec);
	}
}

static ssize_t result_alloc_show(struct class *class, struct class_attribute *attr, char *buf)
{
	strcpy(buf, buffer);
	return strlen(buf);
}

struct class_attribute class_attr_result_alloc = {
	.attr = { .name = "result_alloc", .mode = 0644 },
	.show	= result_alloc_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret = 0;

	attr_class = class_create(THIS_MODULE, "my_sysfs_class");
	if (attr_class == NULL) {
		DBG_ERROR("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_result_alloc);
	if (ret) {
		DBG_ERROR("mymodule: error creating sysfs class attribute - abs_time\n");
		class_destroy(attr_class);
		return ret;
	}

	buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	alloc_tests();

	DBG_INFO("mymodule: module loaded\n");
	return ret;

}

static void mymodule_exit(void)
{

	class_destroy(attr_class);

	if (buffer) {
		kfree(buffer);
	}
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("Memory allocating test kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
