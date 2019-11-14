/*
 * mod_sysfs.c lowercase converter sysfs
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>


static char msg[PAGE_SIZE];

static int RCalls, WCalls, Charc_Processed,Charc_Converted;

static void lowcaseConvert(char *pbuf)
{
	int i = 0;
	while(pbuf[i] != 0)
	{
		if(pbuf[i] >= 'A' && pbuf[i] <= 'Z')
		{
			pbuf[i] = pbuf[i] - 'A' + 'a';
			Charc_Converted++;
		}
		i++;
		Charc_Processed++;
	}
}

static ssize_t param_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	pr_info("mod_sysfs: write data");

	size_t sizeToCopy;
	if (count <= PAGE_SIZE)
		sizeToCopy = count;
	else
		sizeToCopy = PAGE_SIZE - 1;

	int i;
	for(i = 0; i < sizeToCopy; i++)
		arr[i] = buf[i];
	arr[sizeToCopy] = 0;

	WCalls++;

	lowcaseConvert(arr);

	return count;
}

static ssize_t param_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("mod_sysfs: read data");

	strcpy(buf, arr);

	RCalls++;

	return strlen(buf);
}

static ssize_t stat_show(struct class *class, struct class_attribute *attr, char *buf)
{
	char buff[500] = "";
	char buffstat[40];

	strcat(buff, " statistic \n");
	sprintf(buff, "write calls", WCalls);
	sprintf(buff, "read calls", RCalls);
	sprintf(buff, "Characters processed", Charc_Processed);
	sprintf(buff, "Characters converted", Charc_Converted);

	strcpy(buf, buff);

	return strlen(buf);
}

struct class_attribute class_attr_stat = {
	.attr = { .name = "stat", .mode = 0666 },
	.show	= stat_show,
};

static struct class *attr_class = 0;

struct class_attribute class_attr_param = {
	.attr = { .name = "param", .mode = 0666 },
	.show	= param_show,
	.store	= param_store
};

static struct proc_dir_entry *entstat;

//init
static int init_mod(void)
{
	static int RCalls = 0;
	static int WCalls = 0;
	static int Charc_Processed = 0;
	static int Charc_Converted = 0;

	int ret;

	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("mod_sysfs: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_param);
	if (ret) {
		pr_err("mod_sysfs: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("mod_sysfs: error creating sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_param);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mod_sysfs: module loaded\n");
	return 0;
}

//exit
static void exit_mod(void)
{
	class_remove_file(attr_class, &class_attr_stat);
	class_remove_file(attr_class, &class_attr_param);
	class_destroy(attr_class);

	pr_info("mod_sysfs: module exited\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("A simple sysfs kernel module.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
