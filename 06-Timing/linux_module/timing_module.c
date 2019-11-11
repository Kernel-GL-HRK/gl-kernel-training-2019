// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/time.h>

#define BUF_SIZE 81
#define AR_SIZE 600

static unsigned long prev_jiffies;
struct timespec ts;
struct timespec prev_ts;
static int one_read_abs = 0;
static struct timer_list calc_timer;

struct stat_cpu {
	int ofset;
	int count;
	unsigned long array_stat[AR_SIZE];
} stt;


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

static ssize_t abs_time_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{
	getnstimeofday(&ts);
	if (one_read_abs) {
		pr_info("Abs time: %lu.%lu\n", prev_ts.tv_sec,
						prev_ts.tv_nsec);
		sprintf(buf, "Abs time: %lu.%lu\n", prev_ts.tv_sec,
						prev_ts.tv_nsec);
	}
	prev_ts = ts;
	one_read_abs = 1;
	return strlen(buf);
}

static void cpu_calc_function(unsigned long data)
{
	struct file *filp;
	mm_segment_t oldfs;
	char buf[BUF_SIZE];
	unsigned long t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
	unsigned long time_sc_boot, time_idle, time_cpu_usage, cpu_perc;
	char cm[255];

	filp = filp_open("/proc/stat", O_RDONLY, 0);
	oldfs = get_fs();
	set_fs(get_ds());
	if (IS_ERR(filp)) {
		return;
	}
	filp->f_op->read(filp, buf, BUF_SIZE, &filp->f_pos);
	filp_close(filp, NULL);
	set_fs(oldfs);
	pr_info("timing_module: stat: %s\n", buf);
	if (sscanf(buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
	cm, &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10) >= 0) {

		time_sc_boot = t1 + t2 + t3 + t4 + t5 + t6 + t7;
		time_idle = t4 + t5;
		time_cpu_usage = time_sc_boot - time_idle;
		cpu_perc = 100 * time_cpu_usage / time_sc_boot;
		pr_info("results: %lu", cpu_perc);
		stt.array_stat[stt.ofset++] = cpu_perc;
		if (stt.ofset > AR_SIZE) {
			stt.ofset = 0;
		}
		if (stt.count <= AR_SIZE) {
			stt.count++;
		}
	}
	mod_timer(&calc_timer, jiffies + HZ);
}

static ssize_t cpu_load_show(struct class *class,
				struct class_attribute *attr,
						char *buf)
{

	unsigned long res, sum = 0;
	int i;

	for (i = 0; i < stt.count; i++)	{
		sum += stt.array_stat[i];
	}

	res = sum / stt.count;

	sprintf(buf, "Average CPU load: %lu\n", res);

	return strlen(buf);
}

struct class_attribute class_attr_rel_time = {
	.attr = { .name = "rel_time", .mode = 0444 },
	.show	= rel_time_show,
};

struct class_attribute class_attr_abs_time = {
	.attr = { .name = "abs_time", .mode = 0444 },
	.show	= abs_time_show,
};

struct class_attribute class_attr_cpu_load = {
	.attr = { .name = "cpu_load", .mode = 0444 },
	.show	= cpu_load_show,
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

	ret = class_create_file(attr_class, &class_attr_abs_time);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_cpu_load);
	if (ret) {
		pr_err("timing_module: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("timing_module: module loaded\n");

	init_timer(&calc_timer);
	calc_timer.expires = jiffies + HZ;
	calc_timer.function = cpu_calc_function;
	calc_timer.data = 0;
	add_timer(&calc_timer);

	return 0;
}

static void timing_module_exit(void)
{
	del_timer(&calc_timer);
	class_remove_file(attr_class, &class_attr_rel_time);
	class_remove_file(attr_class, &class_attr_abs_time);
	class_remove_file(attr_class, &class_attr_cpu_load);
	class_destroy(attr_class);

	pr_info("timing_module: module exited\n");
}

module_init(timing_module_init);
module_exit(timing_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Timing managment, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
