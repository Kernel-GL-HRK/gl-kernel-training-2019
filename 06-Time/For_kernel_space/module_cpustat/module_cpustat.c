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
struct timer_list mytimer;
int currCpuLoad;
int cpuLoadIsRelaibleVal;

// file work routines
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

// get cpu average load routine
int getCpuStat(int *resultCpuBusyPers)
{
	struct file *myfile = file_open("/proc/stat", O_RDONLY, 0);

	if (myfile == NULL)
		return 0;

	file_read(myfile, 0, arr, 4096);
	file_close(myfile);

	unsigned long cpuBusyTicks[6];
	unsigned long cpuIdleTicks;
	char str[20];

	sscanf(arr, "%s %lu %lu %lu %lu %lu %lu %lu",
	str, &cpuBusyTicks[0], &cpuBusyTicks[1], &cpuBusyTicks[2],
	&cpuIdleTicks, &cpuBusyTicks[3], &cpuBusyTicks[4],
	&cpuBusyTicks[5]);

	unsigned long totalCpuTicks = cpuBusyTicks[0] + cpuBusyTicks[1]
			+ cpuBusyTicks[2] + cpuBusyTicks[3] + cpuBusyTicks[4]
			+ cpuBusyTicks[5] + cpuIdleTicks;

	unsigned long totalCpuBusyTicks = totalCpuTicks - cpuIdleTicks;

	*resultCpuBusyPers = totalCpuBusyTicks * 100 / totalCpuTicks;

	return 1;
}

// timer handler
static void timer_function(unsigned long data)
{
	int result;

	if (getCpuStat(&result)) {
		currCpuLoad = result;
		cpuLoadIsRelaibleVal = 1;
	} else
		cpuLoadIsRelaibleVal = 0;

	mod_timer(&mytimer, jiffies + HZ);
}

static ssize_t cpustat_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	pr_info("mymodule: read data");

	if (cpuLoadIsRelaibleVal)
		sprintf(buf, "Average cpu load: %i%%\n", currCpuLoad);
	else
		sprintf(buf, "Reading error\n");

	return strlen(buf);
}

struct class_attribute class_attr_param = {
	.attr = { .name = "cpustat", .mode = 0666 },
	.show	= cpustat_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	// sysfs attributes configuration
	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_param);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	// timer configuration
	init_timer(&mytimer);
	mytimer.expires = jiffies + HZ;
	mytimer.function = timer_function;
	mytimer.data = NULL;
	add_timer(&mytimer);

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_param);
	class_destroy(attr_class);

	del_timer(&mytimer);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton");
MODULE_DESCRIPTION("Cpu stat Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
