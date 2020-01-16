// SPDX-License-Identifier: GPL
#include <linux/init.h>     // Macros used to mark up functions  __init __exit
#include <linux/module.h>   // Core header for loading LKMs into the kernel
#include <linux/kernel.h>   // Contains types, macros, functions for the kernel
#include <linux/types.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/jiffies.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/time.h>

typedef void *(*memory_allocate) (ssize_t size, void *context);
typedef void (*memory_free) (const void *mem, void *context);
typedef size_t (*memory_incr_size)(const size_t cur_size);

struct cnf_mem_allocate {
	memory_allocate mem_allocate;
	void *cnt;
};

struct cnf_mem_free {
	memory_free mem_free;
	void *cnt;
};

struct mem_config {
	struct cnf_mem_allocate cnf_allocate;
	struct cnf_mem_free	cnf_free;
	memory_incr_size incr_size;
	size_t start_size;
	size_t max_size_mem;
};

struct mem_description {
	struct mem_config config;
	char *buf;
	size_t size_buf;
};

static long long get_timeout(struct timespec start, struct timespec end)
{
	struct timespec sub = timespec_sub(end, start);

	return timespec_to_ns(&sub);
}

static void mem_test(struct mem_description *descr)
{
	struct timespec ts_start;
	struct timespec ts_end;

	long long time_spent_allocate;
	long long time_spent_free;

	size_t size = descr->config.start_size;
	int32_t numb_sample = 0;
	void *res_allocate;

	sprintf(descr->buf + strlen(descr->buf),
		"number     addr            size        alloc(ns)       free(ns)\n");

	while (size < descr->config.max_size_mem) {
		numb_sample++;

		getnstimeofday(&ts_start);
		res_allocate = descr->config.cnf_allocate.mem_allocate(size,
				descr->config.cnf_allocate.cnt);
		getnstimeofday(&ts_end);
		time_spent_allocate = get_timeout(ts_start, ts_end);


		if (res_allocate != NULL) {
			getnstimeofday(&ts_start);
			descr->config.cnf_free.mem_free(res_allocate,
					descr->config.cnf_free.cnt);
			getnstimeofday(&ts_end);
			time_spent_free = get_timeout(ts_start, ts_end);
		} else {
			time_spent_free = 0;
		}

		sprintf(descr->buf + strlen(descr->buf),
				" %4u     %8p         %-10u%10llu     %10llu\n",
				numb_sample, res_allocate, size,
				time_spent_allocate, time_spent_free);
		size = descr->config.incr_size(size);
	}
}

static size_t incr_shift_bit(size_t cur_size)
{
	return (cur_size << 1);
}

struct cnt_kmalloc {
	gfp_t flags;
};

static void *mem_kmalloc(ssize_t size, struct cnt_kmalloc *cnt)
{
	return kmalloc(size, cnt->flags);
}

static void mem_kfree(const void *mem, void *context)
{
	kfree(mem);
}

static ssize_t test_kmalloc_show(struct class *class,
	struct class_attribute *attr, char *buf)
{

	struct mem_description descr;
	struct cnt_kmalloc cnt;

	cnt.flags = GFP_KERNEL;

	descr.config.cnf_allocate.mem_allocate = (memory_allocate)mem_kmalloc;
	descr.config.cnf_allocate.cnt = (void *)&cnt;

	descr.config.cnf_free.mem_free = mem_kfree;
	descr.config.cnf_free.cnt = NULL;

	descr.config.incr_size = incr_shift_bit;

	descr.buf = buf;
	descr.config.max_size_mem = 0x9000000;
	descr.config.start_size = 1;

	sprintf(descr.buf + strlen(descr.buf), "kmalloc:\n");
	mem_test(&descr);


	return strlen(buf);
}



static void *mem_kzalloc(ssize_t size, struct cnt_kmalloc *cnt)
{
	return kzalloc(size, cnt->flags);
}

static void mem_kzfree(const void *mem, void *context)
{
	kzfree(mem);
}

static ssize_t test_kzalloc_show(struct class *class,
	struct class_attribute *attr, char *buf)
{

	struct mem_description descr;
	struct cnt_kmalloc cnt;

	cnt.flags = GFP_KERNEL;

	descr.config.cnf_allocate.mem_allocate = (memory_allocate)mem_kzalloc;
	descr.config.cnf_allocate.cnt = (void *)&cnt;

	descr.config.cnf_free.mem_free = mem_kzfree;
	descr.config.cnf_free.cnt = NULL;

	descr.config.incr_size = incr_shift_bit;

	descr.buf = buf;
	descr.config.max_size_mem = 0x9000000;
	descr.config.start_size = 1;

	sprintf(descr.buf + strlen(descr.buf), "kzmalloc:\n");
	mem_test(&descr);


	return strlen(buf);
}


static void *mem_vmalloc(ssize_t size, void *cnt)
{
	return vmalloc(size);
}

static void mem_vfree(const void *mem, void *context)
{
	vfree(mem);
}

static ssize_t test_vmalloc_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	struct mem_description descr;

	descr.config.cnf_allocate.mem_allocate = (memory_allocate)mem_vmalloc;
	descr.config.cnf_allocate.cnt = NULL;

	descr.config.cnf_free.mem_free = mem_vfree;
	descr.config.cnf_free.cnt = NULL;

	descr.config.incr_size = incr_shift_bit;

	descr.buf = buf;
	descr.config.max_size_mem = 0x9000000;
	descr.config.start_size = 1;

	sprintf(descr.buf + strlen(descr.buf), "vmalloc:\n");
	mem_test(&descr);


	return strlen(buf);
}

struct  mem_free_page_cnt {
	gfp_t flags;
	unsigned long page;
	unsigned int order;
};

static size_t incr_page(size_t cur_size)
{
	return cur_size + PAGE_SIZE;
}

static void *mem_get_fre_page(ssize_t size, struct  mem_free_page_cnt *cnt)
{
	cnt->order = size % PAGE_SIZE == 0 ?
		size / PAGE_SIZE : size / PAGE_SIZE + 1;
	cnt->page = __get_free_pages(cnt->flags, cnt->order);
	return (void *)(cnt->page);
}

static void mem_free_pages(const void *mem, struct mem_free_page_cnt *cnt)
{
	free_pages(cnt->page, cnt->order);
}


static ssize_t test_get_free_page_show(struct class *class,
	struct class_attribute *attr, char *buf)
{

	struct mem_description descr;
	struct mem_free_page_cnt cnt;

	cnt.flags = GFP_KERNEL;

	descr.config.cnf_allocate.mem_allocate =
			(memory_allocate)mem_get_fre_page;

	descr.config.cnf_allocate.cnt = (void *)&cnt;

	descr.config.cnf_free.mem_free = (memory_free)mem_free_pages;
	descr.config.cnf_free.cnt = (void *)&cnt;

	descr.config.incr_size = incr_page;

	descr.buf = buf;
	descr.config.max_size_mem = PAGE_SIZE*29;
	descr.config.start_size = PAGE_SIZE;

	sprintf(descr.buf + strlen(descr.buf), "get_free_page:\n");
	mem_test(&descr);


	return strlen(buf);
}


struct class_attribute attr_test_kmalloc = {
	.attr = { .name = "test_kmalloc", .mode = 0444 },
	.show	= test_kmalloc_show,
};

struct class_attribute attr_test_kzmalloc = {
	.attr = { .name = "test_kzmalloc", .mode = 0444 },
	.show	= test_kzalloc_show,
};

struct class_attribute attr_test_vmalloc = {
	.attr = { .name = "test_vmalloc", .mode = 0444 },
	.show	= test_vmalloc_show,
};

struct class_attribute attr_test_free_page = {
	.attr = { .name = "test_get_free_page", .mode = 0444 },
	.show	= test_get_free_page_show,
};


static struct class *mem_class;

static int __init mem_init(void)
{
int ret;

mem_class = class_create(THIS_MODULE, "memory_management");
if (mem_class == NULL)	{
	pr_err("MemoryManagement: error creating sysfs class\n");
	return -ENOMEM;
}

ret = class_create_file(mem_class, &attr_test_kmalloc);
if (ret) {
	pr_err("MemoryManagement: error creating sysfs class attribute\n");
	return ret;
}

ret = class_create_file(mem_class, &attr_test_kzmalloc);
if (ret) {
	pr_err("MemoryManagement: error creating sysfs class attribute\n");
	return ret;
}

ret = class_create_file(mem_class, &attr_test_vmalloc);
if (ret) {
	pr_err("MemoryManagement: error creating sysfs class attribute\n");
	return ret;
}

ret = class_create_file(mem_class, &attr_test_free_page);
if (ret) {
	pr_err("MemoryManagement: error creating sysfs class attribute\n");
	return ret;
}

pr_info("MemoryManagement:loaded\n");
return 0;
}

static void __exit mem_exit(void)
{
class_remove_file(mem_class, &attr_test_kmalloc);
class_remove_file(mem_class, &attr_test_kzmalloc);
class_remove_file(mem_class, &attr_test_vmalloc);
class_remove_file(mem_class, &attr_test_free_page);
class_destroy(mem_class);
pr_info("MemoryManagement: closed");
}

module_init(mem_init);
module_exit(mem_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Happybolt <dmytro.topikha@gmail.com>");
MODULE_DESCRIPTION("MemoryManagement");
MODULE_VERSION("0.1");
