#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

#define CLASS_NAME	"my_module"
#define CONV_FILE_NAME	"string-conv"
#define LOG_FILE_NAME	"log"

static struct class *attr_class = 0;

static ssize_t rw_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	int ret = 0;
	pr_info("mymodule-sysfs: read file: %s\n", attr->attr.name);

	return ret;
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr,
	const char *buf, size_t count)
{
	pr_info("mymodule-sysfs: write file: %s\n", attr->attr.name);
	pr_info("mymodule-sysfs: write count bytes: %d\n", count);

	return count;
}

/*CLASS_ATTR_RW(rw);*/
struct class_attribute class_attr_sconv = {
	.attr = { .name = CONV_FILE_NAME, .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

struct class_attribute class_attr_log = {
	.attr = { .name = LOG_FILE_NAME, .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

static int __init init_mod(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		pr_err("mymodule-sysfs: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_sconv);
	if (ret) {
		pr_err("mymodule-sysfs: error creating sysfs class attribute %s\n", CONV_FILE_NAME);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_log);
	if (ret) {
		pr_err("mymodule-sysfs: error creating sysfs class attribute %s\n", LOG_FILE_NAME);
		class_remove_file(attr_class, &class_attr_sconv);	
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule-sysfs: module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	class_remove_file(attr_class, &class_attr_log);
	class_remove_file(attr_class, &class_attr_sconv);
	class_destroy(attr_class);
	pr_info("mymodule-sysfs: module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple sysfs module");
MODULE_VERSION("0.1");
