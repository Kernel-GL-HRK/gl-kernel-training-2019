#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/time.h>

struct timespec prevReadAbsTime;

static ssize_t abstime_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	pr_info("mymodule: read data");

	sprintf(buf, "Absolute time of previous reading: %u.%09u seconds\n",
		prevReadAbsTime.tv_sec, prevReadAbsTime.tv_nsec);
	getnstimeofday(&prevReadAbsTime);

	return strlen(buf);
}

struct class_attribute class_attr_abstime = {
	.attr = { .name = "abstime", .mode = 0666 },
	.show	= abstime_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_abstime);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	// Set inititalization time as the first reading absolute time
	getnstimeofday(&prevReadAbsTime);

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_abstime);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton");
MODULE_DESCRIPTION("Absolute time kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
