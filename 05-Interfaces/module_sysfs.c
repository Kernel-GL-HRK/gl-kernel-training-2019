// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

#define DBG_INFO(x, arg...) \
pr_info("My_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("My_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

#define CHAR_SHIFT	32

static char *buffer;
static int ReadCalls;
static int CharsProcessed;
static int CharsConverted;



size_t lowercase(char *pbuf, size_t count)
{
	size_t conv_cnt, i;

	conv_cnt = 0;

	for (i = 1; i < count; i++) {
		CharsProcessed++;
		if ((*pbuf >= 'A') && (*pbuf <= 'Z')) {
			(*pbuf) += CHAR_SHIFT;
			conv_cnt++;
		}
		pbuf++;
	}
	return conv_cnt;
}

static ssize_t statistic_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "read calls = %i\nchars processed = %i\nchars converted = %i\n",
			ReadCalls, CharsProcessed, CharsConverted);
}


static ssize_t string_convertor_show(struct class *class, struct class_attribute *attr, char *buf)
{
	ReadCalls++;
	strcpy(buf, buffer);
	DBG_INFO("buffer = %s", buffer);
	DBG_INFO("buf = %s", buf);
	return strlen(buf);


}
static ssize_t string_convertor_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	strcpy(buffer, buf);
	DBG_INFO("buffer = %s", buffer);
	CharsConverted = lowercase(buffer, count);

	return count;
}

struct class_attribute class_attr_conv = {
	.attr = { .name = "string_convertor", .mode = 0666 },
	.show	= string_convertor_show,
	.store	= string_convertor_store
};

struct class_attribute class_attr_stat = {
	.attr = { .name = "statistic", .mode = 0644 },
	.show	= statistic_show,
};
static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "my_sysfs_class");
	if (attr_class == NULL) {
		DBG_ERROR("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_conv);
	if (ret) {
		DBG_ERROR("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		DBG_ERROR("mymodule: error creating sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_conv);
		class_destroy(attr_class);
		return ret;
	}

	buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);

	ReadCalls = 0;
	CharsProcessed = 0;
	CharsConverted = 0;

	DBG_INFO("mymodule: module loaded\n");
	return 0;

}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_stat);
	class_remove_file(attr_class, &class_attr_conv);
	class_destroy(attr_class);

	if (buffer) {
		kfree(buffer);
	}

	DBG_INFO("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Lapin Vadim <lapin.vadim@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
