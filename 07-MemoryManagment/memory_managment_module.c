#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/math64.h>
#include <linux/sched/loadavg.h>
#include <linux/timer.h>
#include <linux/clocksource.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/time64.h>
#include <linux/tick.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

#define CLASS_NAME	"memory_timing_module"
#define VMALLOC_TEST "vmalloc_test"
#define KZALLOC_TEST "kzalloc_test"
#define KMALLOC_TEST "kmalloc_test"
#define GET_FREE_PAGES_TEST "get_free_pages_test"

#define BUF_LEN 2048

#define DBG_INFO(x, arg...) \
pr_info("Test_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("Test_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

static const unsigned long NS = 1e9;

struct memory_module_data {
	struct class *attr_class;
	char buf_str[BUF_LEN];
	struct timespec time;
	struct timespec prev_time;
};

static struct memory_module_data *module_data;

static unsigned long long absolute_time_calc(struct timespec *cur,
						struct timespec *prev)
{
	unsigned long long time_buf = 0;
	unsigned int time_remainder = 0;

	time_buf = (unsigned long long)(cur->tv_sec) * NS +
						cur->tv_nsec;

	time_buf -= ((unsigned long long)(prev->tv_sec) * NS +
						prev->tv_nsec);

	return time_buf;
}

void kmalloc_time_calc(struct memory_module_data *ptr_data)
{
	char *kmalloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
		"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 0; order < 32; order++) {
			getnstimeofday(&ptr_data->prev_time);
			kmalloc_ptr = kmalloc(2 << order, GFP_KERNEL);
				if (kmalloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
				"%u", 2 << order);

			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			getnstimeofday(&ptr_data->prev_time);
			kfree(kmalloc_ptr);
			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
}

void kzalloc_time_calc(struct memory_module_data *ptr_data)
{
	char *kzalloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
		"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 0; order < 32; order++) {
			getnstimeofday(&ptr_data->prev_time);
			kzalloc_ptr = kzalloc(2 << order, GFP_KERNEL);
				if (kzalloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
				"%u", 2 << order);
			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			getnstimeofday(&ptr_data->prev_time);
			kfree(kzalloc_ptr);
			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
}

void vmalloc_time_calc(struct memory_module_data *ptr_data)
{
	char *vmalloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
		"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 0; order < 32; order++) {
			getnstimeofday(&ptr_data->prev_time);
			vmalloc_ptr = vmalloc(2 << order);
				if (vmalloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
				"%u", 2 << order);
			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			getnstimeofday(&ptr_data->prev_time);
			vfree(vmalloc_ptr);
			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
}

void get_free_pages_time_calc(struct memory_module_data *ptr_data)
{
	char *page_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
		"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 0; order < 32; order++) {
			getnstimeofday(&ptr_data->prev_time);
			page_ptr = (char *)__get_free_pages(GFP_KERNEL, order);
				if (page_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"%u", order);

			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			getnstimeofday(&ptr_data->prev_time);
			free_pages((unsigned long)page_ptr, order);
			getnstimeofday(&ptr_data->time);
			time_ns = absolute_time_calc(&ptr_data->time,
					&ptr_data->prev_time);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
}

static ssize_t time_for_kmalloc(struct class *class,
			struct class_attribute *attr, char *buf)
{
	kmalloc_time_calc(module_data);
	strcpy(buf, module_data->buf_str);

	DBG_INFO("time_for_kmalloc: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

static ssize_t time_for_kzalloc(struct class *class,
			struct class_attribute *attr, char *buf)
{
	kzalloc_time_calc(module_data);
	strcpy(buf, module_data->buf_str);

	DBG_INFO("time_for_kzalloc: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

static ssize_t time_for_vmalloc(struct class *class,
			struct class_attribute *attr, char *buf)
{
	vmalloc_time_calc(module_data);
	strcpy(buf, module_data->buf_str);

	DBG_INFO("time_for_vmalloc: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

static ssize_t time_for_get_free_pages(struct class *class,
			struct class_attribute *attr, char *buf)
{
	get_free_pages_time_calc(module_data);
	strcpy(buf, module_data->buf_str);

	DBG_INFO("time_for_pages: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

struct class_attribute class_attr_time_for_kzalloc = {
	.attr = { .name = KZALLOC_TEST, .mode = 0444 },
	.show	= time_for_kzalloc,
};

struct class_attribute class_attr_time_for_kmalloc = {
	.attr = { .name = KMALLOC_TEST, .mode = 0444 },
	.show	= time_for_kmalloc,
};

struct class_attribute class_attr_time_for_vmalloc = {
	.attr = { .name = VMALLOC_TEST, .mode = 0444 },
	.show	= time_for_vmalloc,
};

struct class_attribute class_attr_time_for_get_free_pages = {
	.attr = { .name = GET_FREE_PAGES_TEST, .mode = 0444 },
	.show	= time_for_get_free_pages,
};

static int __init memory_module_init(void)
{
	int ret;

	module_data = kzalloc(sizeof(struct memory_module_data), GFP_KERNEL);
	module_data->prev_time.tv_nsec = 0;
	module_data->prev_time.tv_sec = 0;
	memset(module_data->buf_str, 0, BUF_LEN);

	module_data->attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (module_data->attr_class == NULL) {
		DBG_ERROR("Error while creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(module_data->attr_class,
						&class_attr_time_for_kzalloc);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	ret = class_create_file(module_data->attr_class,
						&class_attr_time_for_kmalloc);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	ret = class_create_file(module_data->attr_class,
						&class_attr_time_for_vmalloc);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	ret = class_create_file(module_data->attr_class,
				&class_attr_time_for_get_free_pages);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	DBG_INFO("Module is loaded\n");
	return 0;
}

static void __exit memory_module_stop(void)
{
	class_remove_file(module_data->attr_class,
			&class_attr_time_for_kzalloc);
	class_remove_file(module_data->attr_class,
			&class_attr_time_for_kmalloc);
	class_remove_file(module_data->attr_class,
			&class_attr_time_for_vmalloc);
	class_remove_file(module_data->attr_class,
			&class_attr_time_for_get_free_pages);
	class_destroy(module_data->attr_class);

	kfree(module_data);

	DBG_INFO("Module is unloaded\n");
}

module_init(memory_module_init);
module_exit(memory_module_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("Test module for linux kernel");
MODULE_VERSION("0.1");
