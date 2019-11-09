#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>

#define MYDEV_NAME	"TimeManagement"
#define CLASS_NAME	"my_time_mod"
#define ABSTIME_FILE_NAME	"absolute-time"
#define RELTIME_FILE_NAME	"relation-time"
#define LOADCPU_FILE_NAME	"load-cpu"

static struct class *attr_class;

static struct device pdev = { .init_name = MYDEV_NAME };

static ssize_t abstime_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	return 0;
}

static ssize_t reltime_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	return 0;
}

static ssize_t loadcpu_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	return 0;
}

struct class_attribute class_attr_abstime = {
	.attr = { .name = ABSTIME_FILE_NAME, .mode = 0444 },
	.show = abstime_show,
};

struct class_attribute class_attr_reltime = {
	.attr = { .name = RELTIME_FILE_NAME, .mode = 0444 },
	.show = reltime_show,
};

struct class_attribute class_attr_loadcpu = {
	.attr = { .name = LOADCPU_FILE_NAME, .mode = 0444 },
	.show = loadcpu_show,
};

static int __init init_mod(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		dev_err(&pdev, "error creating sysfs class: %s\n",
			CLASS_NAME);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_abstime);
	if (ret) {
		dev_err(&pdev, "error creating sysfs class attribute %s\n",
			ABSTIME_FILE_NAME);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_reltime);
	if (ret) {
		dev_err(&pdev, "error creating sysfs class attribute %s\n",
			RELTIME_FILE_NAME);
		class_remove_file(attr_class, &class_attr_abstime);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_loadcpu);
	if (ret) {
		dev_err(&pdev, "error creating sysfs class attribute %s\n",
			LOADCPU_FILE_NAME);
		class_remove_file(attr_class, &class_attr_reltime);
		class_remove_file(attr_class, &class_attr_abstime);
		class_destroy(attr_class);
		return ret;
	}

	dev_info(&pdev, "module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	class_remove_file(attr_class, &class_attr_abstime);
	class_remove_file(attr_class, &class_attr_reltime);
	class_remove_file(attr_class, &class_attr_loadcpu);
	class_destroy(attr_class);
	dev_info(&pdev, "module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple sysfs time management Linux module");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(MYDEV_NAME);
