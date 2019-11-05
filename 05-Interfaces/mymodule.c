#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

#define CLASS_NAME	"my_module"
#define CONV_FILE_NAME	"string-conv"
#define LOG_FILE_NAME	"log"

static struct class *attr_class;
static char msg[PAGE_SIZE];
static ssize_t totalchar, totalwrite, totalread, totalcharconv, totaldigit;

static ssize_t conv_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	int ret = 0;

	pr_info("mymodule-sysfs: read file: %s\n", attr->attr.name);

	if (msg[0]) {
		ret = sprintf(buf, "%s\n", msg);
		msg[0] = '\0';
	}
	totalread++;

	return ret;
}

static ssize_t conv_store(struct class *class, struct class_attribute *attr,
	const char *buf, size_t count)
{
	int i = 0;

	pr_info("mymodule-sysfs: write file: %s\n", attr->attr.name);
	pr_info("mymodule-sysfs: write count bytes: %ld\n", count);

	sscanf(buf, "%s", msg);
	msg[count] = '\0';
	totalwrite++;
	pr_info("mymodule-sysfs: string: %s\n", msg);
	while (msg[i] != '\0') {
		if (isdigit(msg[i])) {
			totaldigit++;
		} else if (isupper(msg[i])) {
			msg[i] = tolower(msg[i]);
			totalcharconv++;
		}
		i++;
	}
	totalchar += i;
	pr_info("mymodule-sysfs: to uppercase converter: %s\n", msg);

	return count;
}

static ssize_t log_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	int ret = 0;

	pr_info("mymodule-sysfs: read file: %s\n", attr->attr.name);

	ret = sprintf(buf, "Total chars puts: %ld\n", totalchar);
	ret += sprintf(buf + ret, "Total digits puts: %ld\n",
		totaldigit);
	ret += sprintf(buf + ret, "Total chars to convertions: %ld\n",
		totalcharconv);
	ret += sprintf(buf + ret,
		"Total number of write file \"%s\": %ld\n",
		CONV_FILE_NAME, totalwrite);
	ret += sprintf(buf + ret,
		"Total number of read file \"%s\": %ld\n",
		CONV_FILE_NAME, totalread);

	return ret;
}

/*CLASS_ATTR_RW(rw);*/
struct class_attribute class_attr_sconv = {
	.attr = { .name = CONV_FILE_NAME, .mode = 0666 },
	.show	= conv_show,
	.store	= conv_store
};

struct class_attribute class_attr_log = {
	.attr = { .name = LOG_FILE_NAME, .mode = 0644 },
	.show	= log_show,
};

static int __init init_mod(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		pr_err("mymodule-sysfs: error creating sysfs class: %s\n",
			CLASS_NAME);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_sconv);
	if (ret) {
		pr_err("mymodule-sysfs: error creating sysfs class attribute %s\n",
			CONV_FILE_NAME);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_log);
	if (ret) {
		pr_err("mymodule-sysfs: error creating sysfs class attribute %s\n",
			LOG_FILE_NAME);
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
