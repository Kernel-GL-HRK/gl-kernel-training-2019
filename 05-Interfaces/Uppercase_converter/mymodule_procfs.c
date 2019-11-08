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

// routines
static void uppercaseConvert(char *pbuf)
{
	int i = 0;
	while(pbuf[i] != 0)
	{
		if(pbuf[i] >= 'a' && pbuf[i] <= 'z')
		{
			pbuf[i] = pbuf[i] - 'a' + 'A';
		}
		i++;
	}
}

// write parameter function
static ssize_t paramwrite(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	// save data to bufer
	ret = simple_write_to_buffer(msg, sizeof(msg), ppos, pbuf, count);
	pr_info("mymodule_procfs: write to parameter %d bytes \n", ret);
	msg_size = ret;

	// uppercase convertion
	uppercaseConvert(msg);

	return ret;
}

// read parameter function
static ssize_t paramread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	// read data from buffer
	ret = simple_read_from_buffer(pbuf, count, ppos, msg, msg_size);
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

// module initialization
static int mymodule_init(void)
{
	ent = proc_create("uppercaseConvParam", 0666, NULL, &myops);
	if (ent == NULL) {
		pr_err("mymodule_procfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info("mymodule_procfs: module loaded\n");
	return 0;
}

// module exit
static void mymodule_exit(void)
{
	proc_remove(ent);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton");
MODULE_DESCRIPTION("Procfs kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
