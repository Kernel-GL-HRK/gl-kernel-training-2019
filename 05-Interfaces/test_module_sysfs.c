#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define CLASS_NAME	"test_module_sysfs"

#define DBG_INFO(x, arg...) \
pr_info("Test_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("Test_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

struct test_module_data {
	struct class *attr_class;
	char *buffer;
	size_t size_buffer;
	size_t total_calls;
	size_t total_chars;
	size_t conv_chars;
};

static struct test_module_data data = {
	.attr_class = NULL,
	.buffer = NULL,
	.size_buffer = 0,
	.total_calls = 0,
	.total_chars = 0,
	.conv_chars = 0,
};

size_t to_lowercase(char *pbuf, size_t count)
{
	size_t conv_cnt, i;

	conv_cnt = 0;

	for (i = 1; i < count; i++) {
		if ((*pbuf >= 'A') && (*pbuf <= 'Z')) {
			(*pbuf) += 32;
			conv_cnt++;
		}
		pbuf++;
	}
	return conv_cnt;
}

static int value;

static ssize_t conv_show(struct class *class, \
			struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%s\n", data.buffer);

	DBG_INFO("conv_show: buf_len = %d\n", value);
	return strlen(buf);
}

static ssize_t conv_store(struct class *class, \
			struct class_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%s", data.buffer);
	value = strlen(data.buffer);
	data.conv_chars = to_lowercase(data.buffer, count);
	data.total_chars = count - 1;
	data.total_calls++;

	DBG_INFO("conv_store: buf_len = %d\n", value);
	return count;
}

static ssize_t stat_show(struct class *class, \
			struct class_attribute *attr, char *buf)
{
	sprintf(buf, "TOTAL_CONV = %i\nTOTAL_CHARS = %i\nTOTAL_CALLS = %i\n",\
			data.conv_chars, data.total_chars, data.total_calls);

	DBG_INFO("stat_show: buf_len = %d\n", value);

	return strlen(buf);
}

struct class_attribute class_attr_conv = {
	.attr = { .name = "string_conv", .mode = 0666 },
	.show	= conv_show,
	.store	= conv_store
};

struct class_attribute class_attr_stat = {
	.attr = { .name = "total_stat", .mode = 0444 },
	.show	= stat_show,
};

static int __init test_module_init(void)
{
	int ret;

	data.attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (data.attr_class == NULL) {
		DBG_ERROR("Error while creating sysfs class\n");
		return -ENOMEM;
	}


	ret = class_create_file(data.attr_class, &class_attr_conv);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(data.attr_class);
		return ret;
	}

	ret = class_create_file(data.attr_class, &class_attr_stat);
	if (ret) {
		DBG_ERROR("Error while creating sysfs class attribute\n");
		class_destroy(data.attr_class);
		return ret;
	}

	char *buffer;

	buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	data.buffer = buffer;

	DBG_INFO("Module is loaded\n");
	return 0;
}

static void __exit test_module_stop(void)
{
	class_remove_file(data.attr_class, &class_attr_conv);
	class_remove_file(data.attr_class, &class_attr_stat);
	class_destroy(data.attr_class);

		if (data.buffer) {
			kfree(data.buffer);
		}

	DBG_INFO("Module is unloaded\n");
}

module_init(test_module_init);
module_exit(test_module_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("Test module for linux kernel");
MODULE_VERSION("0.1");
