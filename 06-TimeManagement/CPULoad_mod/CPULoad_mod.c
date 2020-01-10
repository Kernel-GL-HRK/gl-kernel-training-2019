/*
 * CPULoad_mod.c  CPU load calc module
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#include <linux/timer.h>

unsigned char arr[4096];
struct timer_list timer1;
int CpuLoadOk;
int CpuLoad;

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

int file_read(struct file *file, unsigned long long offset,
		unsigned char *data, unsigned int size)
{
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(get_ds());

	ret = vfs_read(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}

void file_close(struct file *file)
{
	filp_close(file, NULL);
}

int getCpuStat(int *resultCpuBusyPers)
{
	struct file *myfile = file_open("/proc/stat", O_RDONLY, 0);

	if (myfile == NULL)
		return 0;

	file_read(myfile, 0, arr, 4096);
	file_close(myfile);

	unsigned long cputicks[6];
	unsigned long cpuidle;
	char str[20];

	sscanf(arr, "%s %lu %lu %lu %lu %lu %lu %lu",
	str, &cputicks[0], &cputicks[1], &cputicks[2],
	&cputicks, &cputicks[3], &cputicks[4],
	&cputicks[5]);

	unsigned long totalCpuTicks = cputicks[0] + cputicks[1]
			+ cputicks[2] + cputicks[3] + cputicks[4]
			+ cputicks[5] + cpuidle;

	unsigned long totalCpuBusyTicks = totalCpuTicks - cpuidle;

	*resultCpuBusyPers = totalCpuBusyTicks * 100 / totalCpuTicks;

	return 1;
}

static void timer_func(unsigned long data)
{
	int result;

	if (getCpuStat(&result)) {
		CpuLoad = result;
		CpuLoadOk = 1;
	} else
		CpuLoadOk = 0;

	mod_timer(&timer1, jiffies + HZ);
}

static ssize_t cpustat_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	if (CpuLoadOk)
		sprintf(buf, "Cpu load: %i%%\n", CpuLoad);
	else
		sprintf(buf, "Reading error\n");

	return strlen(buf);
}

struct class_attribute class_attr_param = {
	.attr = { .name = "cpustat", .mode = 0666 },
	.show	= cpustat_show,
};

static struct class *attr_class;

static int loadmodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("loadmodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_param);
	if (ret) {
		pr_err("loadmodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	init_timer(&timer1);
	timer1.expires = jiffies + HZ;
	timer1.function = timer_func;
	timer1.data = NULL;
	add_timer(&timer1);

	pr_info("loadmodule: module loaded\n");
	return 0;
}

static void loadmodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_param);
	class_destroy(attr_class);

	del_timer(&timer1);

	pr_info("loadmodule: module exited\n");
}

module_init(loadmodule_init);
module_exit(loadmodule_exit);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("CPU load calc module");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
