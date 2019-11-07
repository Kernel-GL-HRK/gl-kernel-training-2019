/*
 * mod_procfs.c  uppercase converter using procfs
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

static char msg[PAGE_SIZE];
static ssize_t msg_size;

static int RCalls, WCalls, Charc_Processed,Charc_Converted;

static void uppercaseConvert(char *pbuf)
{
	int i = 0;
	while(pbuf[i] != 0)
	{
		if(pbuf[i] >= 'a' && pbuf[i] <= 'z')
		{
			pbuf[i] = pbuf[i] - 'a' + 'A';
			Charc_Converted++;
		}
		i++;
		Charc_Processed++;
	}
}

static ssize_t paramwrite(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	ret = simple_write_to_buffer(msg, sizeof(msg), ppos, pbuf, count);
	pr_info("mod_procfs: write to parameter %d bytes \n", ret);
	msg_size = ret;

	Charc_Processed = 0;
	Charc_Converted = 0;

	uppercaseConvert(msg);

	WCalls++;

	return ret;
}

static ssize_t paramread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	ret = simple_read_from_buffer(pbuf, count, ppos, msg, msg_size);
	pr_info("mod_procfs: read from parameter %d bytes\n", ret);

	RCalls++;

	return ret;
}

static void strcatInfo(char *pbuf, char *pname, int value)
{
	char tbuf[40] = "|                         |       |\n";

	int i = 0;
	while(pname[i] != 0)
	{
		tbuf[2 + i] = pname[i];
		i++;
	}

	i = 0;
	do
	{
		tbuf[32 - i] = '0' + value % 10;
		value /= 10;
		i++;
	} while (value != 0);

	strcat(pbuf, tbuf);
}

static ssize_t statread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret = 0;

	char buff[500] = "";
	size_t b_size;

	strcat(buff, " statistic \n");
	strcatInfo(buff, "write calls", WCalls);
	strcatInfo(buff, "read calls", RCalls);
	strcatInfo(buff, "Characters processed", Charc_Processed);
	strcatInfo(buff, "Characters converted", Charc_Converted);

	b_size = strlen(buff) + 1;

	ret = simple_read_from_buffer(pbuf, count, ppos, buff, b_size);
	pr_info("mod_procfs: read from parameter %d bytes\n", ret);

	return ret;
}

static struct file_operations myops =
{
	.owner = THIS_MODULE,
	.read = paramread,
	.write = paramwrite,
};

static struct proc_dir_entry *ent;

static struct file_operations myopsstat =
{
	.owner = THIS_MODULE,
	.read = statread,
};

static struct proc_dir_entry *entstat;

//init
static int mymodule_init(void)
{
	static int RCalls = 0;
	static int WCalls = 0;
	static int Charc_Processed = 0;
	static int Charc_Converted = 0;

	ent = proc_create("uppercaseConvParam", 0666, NULL, &myops);
	if (ent == NULL) {
		pr_err("mod_procfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	entstat = proc_create("uppercaseConvStat", 0666, NULL, &myopsstat);
	if(entstat == NULL) {
		proc_remove(ent);
		pr_err("mod_procfs: error creating procfs entry stat\n");
		return -ENOMEM;
	}

	pr_info("mod_procfs: module loaded\n");
	return 0;
}

//exit
static void mymodule_exit(void)
{
	proc_remove(ent);
	proc_remove(entstat);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

module_init(init_mod);
module_exit(exit_mod);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("A simple procfs kernel module.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
