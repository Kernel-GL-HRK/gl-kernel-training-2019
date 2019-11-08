#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

// variables
static char msg[PAGE_SIZE];
static ssize_t msg_size;

static int statTotalReadCalls = 0;
static int statTotalWriteCalls = 0;
static int amountCharactersProcessed = 0;
static int amountCharactersConverted = 0;

// routines
static void uppercaseConvert(char *pbuf)
{
	int i = 0;
	while(pbuf[i] != 0)
	{
		if(pbuf[i] >= 'a' && pbuf[i] <= 'z')
		{
			pbuf[i] = pbuf[i] - 'a' + 'A';
			amountCharactersConverted++;
		}
		i++;
		amountCharactersProcessed++; // statistic	
	}
}

static void drawInfoStatLine(char *pbuf, char *pname, int value)
{
	char tbuf[40] = "|                         |       |\n";
	
	// add name
	int i = 0;
	while(pname[i] != 0)
	{
		tbuf[2 + i] = pname[i];
		i++;
	}
	
	// add value
	i = 0;
	do
	{
		tbuf[32 - i] = '0' + value % 10;
		value /= 10;
		i++;	
	} while (value != 0);
	
	strcat(pbuf, tbuf);
}

// write parameter function
static ssize_t paramwrite(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	// save data to bufer
	ret = simple_write_to_buffer(msg, sizeof(msg), ppos, pbuf, count);
	pr_info("mymodule_procfs: write to parameter %d bytes \n", ret);
	msg_size = ret;

	// reset statistic variables
	amountCharactersProcessed = 0;
	amountCharactersConverted = 0;

	// uppercase convertion
	uppercaseConvert(msg);

	// statistic
	statTotalWriteCalls++;

	return ret;
}

// read parameter function
static ssize_t paramread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	// read data from buffer
	ret = simple_read_from_buffer(pbuf, count, ppos, msg, msg_size);
	pr_info("mymodule_procfs: read from parameter %d bytes\n", ret);

	// statistic 
	statTotalReadCalls++;

	return ret;
}

// read status function
static ssize_t statread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret = 0;

	char buff[500] = "";
	size_t b_size; 

	strcat(buff, "-----------------------------------\n");
	strcat(buff, "| Uppercase convrting statistic   |\n");
	strcat(buff, "-----------------------------------\n");
	drawInfoStatLine(buff, "Total write calls", statTotalWriteCalls);
	strcat(buff, "-----------------------------------\n");
	drawInfoStatLine(buff, "Total read calls", statTotalReadCalls);
	strcat(buff, "-----------------------------------\n");
	drawInfoStatLine(buff, "Characters processed", amountCharactersProcessed);
	strcat(buff, "-----------------------------------\n");
	drawInfoStatLine(buff, "Characters converted", amountCharactersConverted);
	strcat(buff, "-----------------------------------\n");	

	b_size = strlen(buff) + 1;
	
	// read data from buffer
	ret = simple_read_from_buffer(pbuf, count, ppos, buff, b_size);
	pr_info("mymodule_procfs: read from parameter %d bytes\n", ret);

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

// module initialization
static int mymodule_init(void)
{
	ent = proc_create("uppercaseConvParam", 0666, NULL, &myops);
	if (ent == NULL) {
		pr_err("mymodule_procfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	entstat = proc_create("uppercaseConvStat", 0666, NULL, &myopsstat);
	if(entstat == NULL) {
		proc_remove(ent);
		pr_err("mymodule_procfs: error creating procfs entry stat\n");
		return -ENOMEM;
	}

	pr_info("mymodule_procfs: module loaded\n");
	return 0;
}

// module exit
static void mymodule_exit(void)
{
	proc_remove(ent);
	proc_remove(entstat);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton");
MODULE_DESCRIPTION("Procfs kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
