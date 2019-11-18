// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  module_07.c - Test module
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/tick.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/err.h>


#define MAX_POW 33
#define INTERVAL_1NS 1000000000ULL
#define POW2(x) (2UL << (unsigned long)(x))
#define COUNT_STAT_POINT 8

static struct kobject *kobject_module_07;
static char show_buf[PAGE_SIZE];
static unsigned long mem_unit;

enum STAT_NAME {
	NAME_KMALLOC = 0,
	NAME_KFREE,
	NAME_VMALLOC,
	NAME_VFREE,
	NAME_KZALLOC,
	NAME_KZFREE,
	NAME_GET_FREE_PAGES,
	NAME_FREE_PAGES
};

static char *statistics_name[] = {
	"kmalloc", "kfree",
	"vmalloc", "vfree",
	"kzmalloc", "kzfree",
	"get_free_pages", "free_pages"
};

struct statistic_info {
	unsigned long long dt[COUNT_STAT_POINT];
};

static struct statistic_info statistics[MAX_POW];

inline unsigned long long calck_time(const struct timespec *begin,
	const struct timespec *end)
{
	return (end->tv_sec * INTERVAL_1NS + end->tv_nsec) -
			(begin->tv_sec * INTERVAL_1NS + begin->tv_nsec);
}

static char *fun_get_free_pages(size_t n)
{
	unsigned int order = (unsigned int)n;

	return  (char *)__get_free_pages(GFP_KERNEL, order);
}

static void fun_free_pages(char *p, size_t n)
{
	free_pages((unsigned long)p, n);
}

static char *fun_kzalloc(size_t n)
{
	return  kzalloc(n, GFP_KERNEL);
}

static void fun_kzfree(char *p, size_t n)
{
	kzfree(p);
}


static char *fun_vmalloc(size_t n)
{
	return  vmalloc(n);
}

static void fun_vfree(char *p, size_t n)
{
	vfree(p);
}

static char *fun_kmalloc(size_t n)
{
	return  kmalloc(n, GFP_KERNEL);
}

static void fun_kfree(char *p, size_t n)
{
	kfree(p);
}

static void test_mem(
	char * (*fun_alloc)(size_t),
	void (*fun_free)(char *, size_t),
	enum STAT_NAME name_alloc,
	enum STAT_NAME name_free
	)
{

	char *buf;
	struct timespec tim_begin, tim_end;

	unsigned int pow = 0;

	for (pow = 0; pow < MAX_POW; pow++) {

		size_t new_size = POW2(pow);

		tim_begin.tv_sec = 0;
		tim_begin.tv_nsec = 0;
		tim_end.tv_sec = 0;
		tim_end.tv_nsec = 0;

		if (name_alloc == NAME_GET_FREE_PAGES) {

			getnstimeofday(&tim_begin);
			buf = fun_alloc(get_order(new_size));
			getnstimeofday(&tim_end);

		} else {

			getnstimeofday(&tim_begin);
			buf = fun_alloc(new_size);
			getnstimeofday(&tim_end);

		}

		if (buf == NULL) {
			pr_err("%s:%d error\n",  __func__, __LINE__);
			return;
		}

		statistics[pow].dt[name_alloc] =
			calck_time(&tim_begin, &tim_end);

		tim_begin.tv_sec = 0;
		tim_begin.tv_nsec = 0;
		tim_end.tv_sec = 0;
		tim_end.tv_nsec = 0;


		if (name_alloc == NAME_GET_FREE_PAGES) {

			getnstimeofday(&tim_begin);
			fun_free(buf, get_order(new_size));
			getnstimeofday(&tim_end);

		} else {

			getnstimeofday(&tim_begin);
			fun_free(buf, 0);
			getnstimeofday(&tim_end);
			buf = 0;

		}

		statistics[pow].dt[name_free] =
				calck_time(&tim_begin, &tim_end);
	}
}

static ssize_t mem_test_show(struct kobject *kobj,
	struct kobj_attribute *attr,
		      char *buf)
{
	int i;

	test_mem(fun_kmalloc, fun_kfree, NAME_KMALLOC, NAME_KFREE);
	test_mem(fun_vmalloc, fun_vfree, NAME_VMALLOC, NAME_VFREE);
	test_mem(fun_kzalloc, fun_kzfree, NAME_KZALLOC, NAME_KZFREE);
	test_mem(fun_get_free_pages, fun_free_pages,
		NAME_GET_FREE_PAGES, NAME_FREE_PAGES);

	sprintf(show_buf,
		"%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s\n",
		"size/dt",
		statistics_name[NAME_KMALLOC],
		statistics_name[NAME_KFREE],
		statistics_name[NAME_VMALLOC],
		statistics_name[NAME_VFREE],
		statistics_name[NAME_KZALLOC],
		statistics_name[NAME_KZFREE],
		statistics_name[NAME_GET_FREE_PAGES],
		statistics_name[NAME_FREE_PAGES]
		);

	strcat(buf, show_buf);

	for (i = 0; i < MAX_POW; ++i) {

		sprintf(show_buf,
			"%-12lu%-12llu%-12llu%-12llu%-12llu%-12llu%-12llu%-12llu%-12llu\n",
			POW2(i),
			statistics[i].dt[NAME_KMALLOC],
			statistics[i].dt[NAME_KFREE],
			statistics[i].dt[NAME_VMALLOC],
			statistics[i].dt[NAME_VFREE],
			statistics[i].dt[NAME_KZALLOC],
			statistics[i].dt[NAME_KZFREE],
			statistics[i].dt[NAME_GET_FREE_PAGES],
			statistics[i].dt[NAME_FREE_PAGES]
			);


		strcat(buf, show_buf);
	}

	return sprintf(buf, "%s", buf);
}

static struct kobj_attribute mem_test_attribute =
	__ATTR(mem_test, 0444, mem_test_show, NULL);

static struct attribute *attrs[] = {
	&mem_test_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int InitSysFS(void)
{
	int ret;

	// /sys/module_07/
	kobject_module_07 = kobject_create_and_add("module_07", NULL);
	if (!kobject_module_07)
		return -ENOMEM;

	ret = sysfs_create_group(kobject_module_07, &attr_group);
	if (ret)
		kobject_put(kobject_module_07);
	return ret;
}

static int module_07(void)
{
	int ret;
	struct sysinfo sys_info;

	ret = InitSysFS();
	if (ret)
		return ret;

	si_meminfo(&sys_info);

	mem_unit = sys_info.mem_unit;

	pr_info("totalram: %lu, freeram: %lu, mem_unit: %u\n",
		sys_info.totalram, sys_info.freeram,
		sys_info.mem_unit);

	pr_info("%s: module starting\n",  __func__);

	return 0;
}

static void module_07_exit(void)
{
	sysfs_remove_group(kobject_module_07, &attr_group);
	kobject_put(kobject_module_07);

	pr_info("%s: module exit\n",  __func__);

}

module_init(module_07);
module_exit(module_07_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("A simple Kernel module.");
MODULE_VERSION("0.1");
