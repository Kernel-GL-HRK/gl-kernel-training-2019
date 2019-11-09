#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/types.h>

static u64 timestamp;

static ssize_t ro_show(struct class *class,
		struct class_attribute *attr,
		char *buf)
{
	u64 temp, delta;

	temp = get_jiffies_64();
	delta = temp - timestamp;
	timestamp = temp;

	char text[100];

	sprintf(text, "%lld %s %d\n", delta,
		"ticks counted since last read on freq", HZ);

	strcpy(buf, text);
	return strlen(text);
}

struct class_attribute class_attr_ro = { .attr = { .name = "timestamp",
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

	timestamp = get_jiffies_64();
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_ro);
	class_destroy(attr_class);
	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Yuriy Podgorniy");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
