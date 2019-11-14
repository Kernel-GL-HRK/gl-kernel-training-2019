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
#include <linux/tick.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define CLASS_NAME	"timers_module"
#define RELATIVE_TIME "relative_time"
#define ABSOLUTE_TIME "absolute_time"
#define AVERAGE_LOAD "average_load"

#define DBG_INFO(x, arg...) \
pr_info("Test_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("Test_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

static const unsigned long NS = 1e9;
static const unsigned long MS = 1e3;

struct timers_module_data {
	struct class *attr_class;
	char buf_str[32];
	struct timer_list timer;
	struct timespec time;
	struct timespec prev_time;
	unsigned long prev_jiffies;
	unsigned long cur_jiffies;
	int first_abs_reading;
	int first_rel_reading;
};

struct module_time {
	unsigned long s;
	unsigned long ms;
};

static struct timers_module_data *module_data;

static void absolute_time_calc(struct timers_module_data *ptr,
						struct timespec *ptr_timespec)
{
	unsigned long long time_buf = 0;
	unsigned int time_remainder = 0;

	time_buf = (unsigned long long)(ptr->time.tv_sec) * NS +
						ptr->time.tv_nsec;

	time_buf -= ((unsigned long long)(ptr->prev_time.tv_sec) * NS +
						ptr->prev_time.tv_nsec);

	ptr->prev_time.tv_sec = ptr->time.tv_sec;
	ptr->prev_time.tv_nsec = ptr->time.tv_nsec;
	ptr_timespec->tv_sec = (long)(div_u64_rem(time_buf,
						NS, &time_remainder));

	ptr_timespec->tv_nsec = time_remainder;
}

static void relative_time_calc(struct timers_module_data *ptr,
						 struct module_time *ptr_time)
{
	unsigned long buf_jiffies;

	buf_jiffies = jiffies;
	ptr->cur_jiffies = buf_jiffies - ptr->prev_jiffies;
	ptr->prev_jiffies = buf_jiffies;
	ptr_time->s = jiffies_to_msecs(ptr->cur_jiffies) / 1000;
	ptr_time->ms = jiffies_to_msecs(ptr->cur_jiffies) % 1000;
}

static void time_function(unsigned long data)
{
	mod_timer(&module_data->timer, jiffies + msecs_to_jiffies(1000));

	struct file *f;
	char buf_file[64] = {0};
	char *buf_file_sub;
	mm_segment_t fs;

	buf_file_sub = buf_file;
	f = filp_open("/proc/loadavg", O_RDONLY, 0);

	if (f == NULL) {
		DBG_ERROR("Error while open file!\n");
		return;
	} else {
		fs = get_fs();
		set_fs(get_ds());
		f->f_op->read(f, buf_file, 64, &f->f_pos);
		set_fs(fs);
	}
	filp_close(f, NULL);

	buf_file_sub = strsep(&buf_file_sub, " ");
	strcpy(module_data->buf_str, buf_file_sub);
}

static ssize_t relative_time_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	struct module_time relative_time = {0, 0};

	relative_time_calc(module_data, &relative_time);

		if (module_data->first_rel_reading == 0) {
			relative_time.s = 0;
			relative_time.ms = 0;
			module_data->first_rel_reading = 1;
		}

	sprintf(buf, "%lu.%03lu s\n", relative_time.s,
							relative_time.ms);

	DBG_INFO("relative_time: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

static ssize_t absolute_time_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	struct timespec absolute_time;

	getnstimeofday(&module_data->time);
	absolute_time_calc(module_data, &absolute_time);

		if (module_data->first_abs_reading == 0) {
			absolute_time.tv_sec = 0;
			absolute_time.tv_nsec = 0;
			module_data->first_abs_reading = 1;
		}

	sprintf(buf, "%lu.%09lu s\n", absolute_time.tv_sec,
							absolute_time.tv_nsec);

	DBG_INFO("absolute_time: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

static ssize_t average_load_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%s\n", module_data->buf_str);

	DBG_INFO("average_load: buf_len = %d\n", strlen(buf));

	return strlen(buf);
}

struct class_attribute class_attr_relative_time = {
	.attr = { .name = RELATIVE_TIME, .mode = 0444 },
	.show	= relative_time_show,
};

struct class_attribute class_attr_absolute_time = {
	.attr = { .name = ABSOLUTE_TIME, .mode = 0444 },
	.show	= absolute_time_show,
};

struct class_attribute class_attr_average_load = {
	.attr = { .name = AVERAGE_LOAD, .mode = 0444 },
	.show	= average_load_show,
};

static int __init timers_module_init(void)
{
	int ret;

	module_data = kzalloc(sizeof(struct timers_module_data), GFP_KERNEL);
	module_data->prev_time.tv_nsec = 0;
	module_data->prev_time.tv_sec = 0;
	module_data->first_abs_reading = 0;
	module_data->first_rel_reading = 0;
	memset(module_data->buf_str, 0, 32);

	init_timer(&module_data->timer);
	module_data->timer.expires = jiffies + HZ;
	module_data->timer.function = time_function;
	module_data->timer.data = 0;
	add_timer(&module_data->timer);

	module_data->attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (module_data->attr_class == NULL) {
		DBG_ERROR("Error while creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(module_data->attr_class,
						&class_attr_relative_time);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	ret = class_create_file(module_data->attr_class,
						&class_attr_absolute_time);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	ret = class_create_file(module_data->attr_class,
						&class_attr_average_load);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(module_data->attr_class);
		return ret;
	}

	DBG_INFO("Module is loaded\n");
	return 0;
}

static void __exit timers_module_stop(void)
{
	class_remove_file(module_data->attr_class, &class_attr_relative_time);
	class_remove_file(module_data->attr_class, &class_attr_absolute_time);
	class_remove_file(module_data->attr_class, &class_attr_average_load);
	class_destroy(module_data->attr_class);
	del_timer(&module_data->timer);

	kfree(module_data);

	DBG_INFO("Module is unloaded\n");
}

module_init(timers_module_init);
module_exit(timers_module_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("Test module for linux kernel");
MODULE_VERSION("0.1");
