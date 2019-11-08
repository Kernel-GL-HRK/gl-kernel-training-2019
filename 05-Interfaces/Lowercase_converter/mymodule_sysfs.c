#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

char arr[PAGE_SIZE];

// routines
static void lowercaseConvert(char *pbuf)
{
	int i = 0;
	while(pbuf[i] != 0)
	{
		if(pbuf[i] >= 'A' && pbuf[i] <= 'Z')
		{
			pbuf[i] = pbuf[i] - 'A' + 'a';
		}
		i++;
	}
}

static ssize_t param_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("mymodule: read data");

	strcpy(buf, arr);

	return strlen(buf);
}

static ssize_t param_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{	
	pr_info("mymodule: write data");
	
	size_t sizeToCopy;
	if (count <= PAGE_SIZE)
		sizeToCopy = count;
	else 
		sizeToCopy = PAGE_SIZE - 1;

	int i;
	for(i = 0; i < sizeToCopy; i++)
		arr[i] = buf[i];
	arr[sizeToCopy] = 0;

	lowercaseConvert(arr); // convert

	return count;
}

struct class_attribute class_attr_param = {
	.attr = { .name = "param", .mode = 0666 },
	.show	= param_show,
	.store	= param_store
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

	ret = class_create_file(attr_class, &class_attr_param);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_param);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton");
MODULE_DESCRIPTION("Sysfs Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
