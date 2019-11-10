#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/string.h>

static struct timer_list timer;
static int usage;
static int lastAllTime;
static int lastUsageTime;

enum contentOfString_enum {
	cpu = 0,
	user,
	nice,
	system,
	idle,
	iowait,
	irq,
	softirq,
	steal,
	guest,
	guest_nice,
};

static int stats[guest_nice + 1];

static void timeout(unsigned long val)
{
	struct file *f;
	char buf[128];
	mm_segment_t fs;

	/**read part of file*/
	f = filp_open("/proc/stat", O_RDONLY, 0);
	if (f == NULL)
		pr_alert("filp_open error!!.\n");
	else{
		fs = get_fs();
		set_fs(get_ds());
		f->f_op->read(f, buf, 128, &f->f_pos);
		set_fs(fs);
	}
	filp_close(f, NULL);

	/**collect data*/
	int contentSwitcher = user;
	/**because /proc/stat starts with "cpu  465 468...."*/
	char *pch = buf + 5;

	while (contentSwitcher <= steal) {
		/**'\n -> \0'*/
		char *tmp = strstr(pch, " ");
		*tmp = '\0';

		if (contentSwitcher != cpu)
			stats[contentSwitcher] = kstrtol(pch, &tmp, 10);

		pch = tmp + 1;
		contentSwitcher++;
	}

	int allTime = stats[user] + stats[nice] + stats[system] + stats[idle]
		+ stats[iowait] + stats[irq] + stats[softirq] + stats[steal];
	int usageTime = stats[user] + stats[nice] + stats[system] + stats[irq]
		+ stats[softirq] + stats[steal];

	// pr_info("user = %d", stats[user]);
	// pr_info("nice = %d", stats[nice]);
	// pr_info("system = %d", stats[system]);
	// pr_info("dile = %d", stats[idle]);
	// pr_info("iowait = %d", stats[iowait]);
	// pr_info("irq = %d", stats[irq]);
	// pr_info("sodtirq = %d", stats[softirq]);
	// pr_info("steal = %d", stats[steal]);

	if (lastAllTime == 0)
		lastAllTime = allTime;

	if (lastUsageTime == 0)
		lastUsageTime = usageTime;

	int deltaAllTime = allTime - lastAllTime;
	int deltaUsageTime = usageTime - lastUsageTime;

	lastUsageTime = usageTime;
	lastAllTime = allTime;

	// pr_info("usageTime = %d", usageTime);
	// pr_info("allTime = %d", allTime);
	// pr_info("deltaUsageTime = %d", deltaUsageTime);
	// pr_info("deltaAllTime = %d", deltaAllTime);

	if (deltaAllTime != 0)
		usage = deltaUsageTime * 10000 / deltaAllTime;

	// pr_info("usage(%%) * 100 = %d", usage);

	/**restart timer*/
	mod_timer(&timer, jiffies + HZ);
}


static ssize_t ro_show(struct class *class,
		struct class_attribute *attr,
		char *buf)
{
	char text[100];

	int integer, fractional;

	integer = usage / 100;
	fractional = usage % 100;

	sprintf(text, "%d.%02d\n", integer, fractional);
	strcpy(buf, text);
	pr_info("text: %s", text);

	return strlen(text);
}

struct class_attribute class_attr_ro = { .attr = { .name = "time",
		.mode = 0444 }, .show = ro_show, };

static struct class *attr_class;

static int mymodule_init(void)
{
	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	int ret = class_create_file(attr_class, &class_attr_ro);

	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule: module loaded\n");

	init_timer(&timer);
	timer.expires = jiffies + HZ;
	timer.function = timeout;
	timer.data = 0;

	add_timer(&timer);

	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_ro);
	class_destroy(attr_class);
	del_timer(&timer);
	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Yuriy Podgorniy");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
