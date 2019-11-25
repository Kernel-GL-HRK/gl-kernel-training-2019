// SPDX-License-Identifier: GPL
#include <linux/init.h>     // Macros used to mark up functions  __init __exit
#include <linux/module.h>   // Core header for loading LKMs into the kernel
#include <linux/kernel.h>   // Contains types, macros, functions for the kernel
#include <linux/types.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/jiffies.h>
#include <linux/sysfs.h>
#include <linux/time.h>
#include <linux/fs.h>

static unsigned long prev_time_jiffies;

static ssize_t relation_time_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	unsigned long cur_time = jiffies;
	unsigned long diff_time = 0;

	diff_time = prev_time_jiffies == 0 ?
		0 : cur_time - prev_time_jiffies;

	pr_info("TimingManagement: Previous relation time: %lu\n",
		prev_time_jiffies);
	pr_info("TimingManagement: Curent relation time: %lu\n", cur_time);
	pr_info("TimingManagement: Different relation time: %lu\n", diff_time);
	prev_time_jiffies = cur_time;
	sprintf(buf, "Diff rel time: %lu\n", diff_time);
	return strlen(buf);
}

struct class_attribute attr_relation_time = {
	.attr = { .name = "relation_time", .mode = 0444 },
	.show	= relation_time_show,
};


static ssize_t absolute_time_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	static struct timespec prev_ts;

	pr_info("TimingManagement:PrevAbsolute time: %lu.%09lu\n", prev_ts.tv_sec,
		prev_ts.tv_nsec);
	sprintf(buf, "TimingManagement:PrevAbsolute time: %lu.%09lu\n",
		prev_ts.tv_sec, prev_ts.tv_nsec);

	getnstimeofday(&prev_ts);

	return strlen(buf);
}

struct class_attribute attr_absolute_time = {
	.attr = { .name = "absolute_time", .mode = 0444 },
	.show	= absolute_time_show,
};


static char buf_cpu[100];

static ssize_t cpu_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	pr_info("TimingManagement:CPU load: %s\n", buf_cpu);
	sprintf(buf, "TimingManagement:CPU load: %s\n", buf_cpu);

	return strlen(buf);
}

struct timer_list timer_update_cpu;

static void update_load_cpu(unsigned long data)
{
	struct file *filp;

	filp = filp_open("proc/loadavg", O_RDONLY, 0);
	if (filp == NULL) {
		pr_err("TimingManagement:can not file open\n");
	} else {
		filp->f_op->read(filp, buf_cpu, sizeof(buf_cpu), &filp->f_pos);
		filp_close(filp, NULL);
	}

	mod_timer(&timer_update_cpu, jiffies + HZ);
}


struct class_attribute class_attr_cpu = {
	.attr = { .name = "cpu", .mode = 0444 },
	.show	= cpu_show,
};

static struct class *attr_class;

static int __init conv_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "timing_management");
	if (attr_class == NULL)	{
		pr_err("TimingManagement: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &attr_relation_time);
	if (ret) {
		pr_err("TimingManagement: error creating sysfs class attribute\n");
		return ret;
	}

	ret = class_create_file(attr_class, &attr_absolute_time);
	if (ret) {
		class_destroy(attr_class);
		pr_err("TimingManagement: error creating sysfs class attribute\n");
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_cpu);
	if (ret) {
		class_destroy(attr_class);
		pr_err("TimingManagement: error creating sysfs class attribute\n");
		return ret;
	}

	init_timer(&timer_update_cpu);
	timer_update_cpu.expires = jiffies + HZ;
	timer_update_cpu.function = update_load_cpu;
	//timer_update_cpu.data = NULL;
	add_timer(&timer_update_cpu);

	pr_info("TimingManagement:loaded\n");
	return 0;
}

static void __exit conv_exit(void)
{
	del_timer(&timer_update_cpu);
	class_remove_file(attr_class, &attr_relation_time);
	class_remove_file(attr_class, &attr_absolute_time);
	class_destroy(attr_class);
	pr_info("TimingManagement: closed");
}

module_init(conv_init);
module_exit(conv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Happybolt <dmytro.topikha@gmail.com>");
MODULE_DESCRIPTION("Timing Management");
MODULE_VERSION("0.1");
