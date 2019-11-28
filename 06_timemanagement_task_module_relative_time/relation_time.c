#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/types.h>


#define NAME_MODULE "Module for relation time"
#define NAME_CLASS "relation_time"
#define NAME_WHO "Vladyslav Kryzhanovskyi"

static u64 time_stamp;
static ssize_t read_only_show(struct class *class,
		struct class_attribute *attr,
		char *buf);

struct class_attribute template_count = { .attr = { .name = "time_stamp",
		.mode = 0444 }, .show = read_only_show, };

static struct class *attr_class;

static int mymodule_init(void)
{
	attr_class = class_create(THIS_MODULE, NAME_CLASS);
	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", NAME_MODULE);
		return -ENOMEM;

	}

	int ret = class_create_file(attr_class, &template_count);

	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n"
			, NAME_MODULE);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("%s: module loaded\n", NAME_MODULE);

	time_stamp = get_jiffies_64();
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &template_count);
	class_destroy(attr_class);
	pr_info("%s: module exited\n", NAME_MODULE);
}



static ssize_t read_only_show(struct class *class,
		struct class_attribute *attr,
		char *buf)
{
	u64 current_time, delta;
	char text[120];

	current_time = get_jiffies_64();
	delta = current_time - time_stamp;
	time_stamp = current_time;
	
	sprintf(text,
	 "At a frequency %d Hz, the number %lld of pulses  between reads.\n"
	  ,HZ ,delta);

	strcpy(buf, text);
	return strlen(text);
}


module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("NAME_WHO");
MODULE_DESCRIPTION("Use sysfs kernel module for relation time.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
