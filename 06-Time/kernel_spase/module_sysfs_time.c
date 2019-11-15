// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

#define DBG_INFO(x, arg...) \
pr_info("My_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("My_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

#define FILE_NAME "/proc/loadavg"
#define DELAY_NS 1000e6

static char *buffer;
static u64 old_jif64;
static struct hrtimer hr_timer;
static struct file *f;

struct file *file_open(const char *path, int flags, int rights)
{
	struct file *filp = NULL;
	mm_segment_t oldfs;
	int err = 0;

	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open(path, flags, rights);
	set_fs(oldfs);
	if (IS_ERR(filp)) {
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

void file_close(struct file *file)
{
	filp_close(file, NULL);
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(get_ds());

	ret = vfs_read(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}

enum hrtimer_restart hrtimer_callback(struct hrtimer *timer_for_restart)
{
	ktime_t currtime, interval;
	loff_t file_offset = 0;

	if (f != NULL) {
		file_read(f, file_offset, buffer, 64);
	}
	DBG_INFO("buffer = %s", buffer);


	currtime  = ktime_get();
	interval = ktime_set(0, DELAY_NS);
	hrtimer_forward(timer_for_restart, currtime, interval);

	return HRTIMER_RESTART;
}

static ssize_t abs_time_show(struct class *class, struct class_attribute *attr, char *buf)
{
	int ret = 0;
	struct timeval tv;

	do_gettimeofday(&tv);
	ret = sprintf(buf, "%ld.%06ld\n", tv.tv_sec, tv.tv_usec);

	return ret;
}

static ssize_t rel_time_show(struct class *class, struct class_attribute *attr, char *buf)
{
	int ret = 0;
	struct timeval tv;
	u64 jif64 = 0;
	u64 rel_time = 0;

	jif64 = get_jiffies_64();
	rel_time = jif64 - old_jif64;
	jiffies_to_timeval(rel_time, &tv);
	ret = sprintf(buf, "%ld.%06ld\n", tv.tv_sec, tv.tv_usec);
	old_jif64 = jif64;

	return ret;
}

static ssize_t cpu_load_show(struct class *class, struct class_attribute *attr, char *buf)
{
	strcpy(buf, buffer);
	return strlen(buf);
}

struct class_attribute class_attr_abs_time = {
	.attr = { .name = "abs_time", .mode = 0644 },
	.show	= abs_time_show,
};
struct class_attribute class_attr_rel_time = {
	.attr = { .name = "rel_time", .mode = 0644 },
	.show	= rel_time_show,
};
struct class_attribute class_attr_cpu_load = {
	.attr = { .name = "cpu_load", .mode = 0644 },
	.show	= cpu_load_show,
};
static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret = 0;
	ktime_t ktime;

	attr_class = class_create(THIS_MODULE, "my_sysfs_class");
	if (attr_class == NULL) {
		DBG_ERROR("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_abs_time);
	if (ret) {
		DBG_ERROR("mymodule: error creating sysfs class attribute - abs_time\n");
		class_destroy(attr_class);
		return ret;
	}
	ret = class_create_file(attr_class, &class_attr_rel_time);
	if (ret) {
		DBG_ERROR("mymodule: error creating sysfs class attribute - rel_time\n");
		class_destroy(attr_class);
		return ret;
	}
	ret = class_create_file(attr_class, &class_attr_cpu_load);
	if (ret) {
		DBG_ERROR("mymodule: error creating sysfs class attribute - cpu_load\n");
		class_destroy(attr_class);
		return ret;
	}

	ktime = ktime_set(0, DELAY_NS);
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &hrtimer_callback;
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);

	f = file_open(FILE_NAME, O_RDONLY, 0);
	if (f == NULL) {
		DBG_ERROR("mymodule: Error while open file!\n");
	} else {
		DBG_INFO("mymodule: Open-OK\n");
	}

	buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);

	DBG_INFO("mymodule: module loaded\n");
	return ret;

}

static void mymodule_exit(void)
{
	int ret = 0;

	file_close(f);
	class_destroy(attr_class);

	if (buffer) {
		kfree(buffer);
	}
	ret = hrtimer_cancel(&hr_timer);
	if (ret)
		DBG_INFO("The timer was still in use...\n");
	DBG_INFO("HR Timer module uninstalling\n");
	DBG_INFO("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Lapin Vadim <lapin.vadim@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
