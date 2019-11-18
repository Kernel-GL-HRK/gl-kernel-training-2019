#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#define MYDEV_NAME	"MemoryManagement"
#define SYSDIR_NAME	"my_memory_management"
#define KMALLOC_FILE_NAME	"kmalloc-test"
#define KZALLOC_FILE_NAME	"kzalloc-test"
#define VMALLOC_FILE_NAME	"vmalloc-test"
#define GETFREEPAGES_FILE_NAME	"get_free_pages-test"
#define SEC_TO_NS(x) (x * (time_t)1E9L)

static struct device pdev = { .init_name = MYDEV_NAME };
static struct kobject *mykobj;

static ssize_t show_mem(struct kobject *kobj, struct attribute *attr, char *buf)
{
	int ret = 0;
	u64 allocsize = 0, relalloc = 0, relfree = 0;
	int i = 0, j = 0;
	void *p = NULL;
	u64 j1 = 0, j2 = 0;
	struct timespec ts = {};

	ret = sprintf(buf, "Type mem: %s\n", attr->name);
	ret += sprintf(buf+ret,
		"2^X\tBuffer size(bytes)\tAllocation time(nS)\tFreeing time(nS)\n");

	for (i = 0, j = 0; i < 64; j++) {
		if (j == 2) {
			i++;
			j = 0;
		}

		if (strcmp(attr->name, GETFREEPAGES_FILE_NAME) == 0)
			allocsize = i;
		else
			allocsize = (1 << i) + j;

		getrawmonotonic(&ts);
		j1 = SEC_TO_NS(ts.tv_sec) + ts.tv_nsec;

		if (strcmp(attr->name, KMALLOC_FILE_NAME) == 0)
			p = kmalloc(allocsize, GFP_KERNEL);
		else if (strcmp(attr->name, KZALLOC_FILE_NAME) == 0)
			p = kzalloc(allocsize, GFP_KERNEL);
		else if (strcmp(attr->name, VMALLOC_FILE_NAME) == 0)
			p = vmalloc(allocsize);
		else if (strcmp(attr->name, GETFREEPAGES_FILE_NAME) == 0) {
			if (j == 0)
				p = (char *)__get_free_pages(GFP_KERNEL,
						allocsize);
			else
				continue;
		}

		ret += sprintf(buf+ret, "2^%d+%d", i, j);

		if (!p) {
			if (strcmp(attr->name, GETFREEPAGES_FILE_NAME) == 0)
				ret += sprintf(buf+ret, "%15ld\t\t",
						(1 << allocsize) * PAGE_SIZE);
			else
				ret += sprintf(buf+ret, "%15lld\t\t",
					allocsize);
			ret += sprintf(buf+ret, "Mem error\n");
			break;
		}

		getrawmonotonic(&ts);
		j2 = SEC_TO_NS(ts.tv_sec) + ts.tv_nsec;
		relalloc = j2 - j1;

		if (strcmp(attr->name, KMALLOC_FILE_NAME) == 0)
			kfree(p);
		else if (strcmp(attr->name, KZALLOC_FILE_NAME) == 0)
			kzfree(p);
		else if (strcmp(attr->name, VMALLOC_FILE_NAME) == 0)
			vfree(p);
		else if (strcmp(attr->name, GETFREEPAGES_FILE_NAME) == 0)
			free_pages((unsigned long)p, allocsize);

		getrawmonotonic(&ts);
		j1 = SEC_TO_NS(ts.tv_sec) + ts.tv_nsec;
		relfree = j1 - j2;

		if (strcmp(attr->name, GETFREEPAGES_FILE_NAME) == 0)
			allocsize = (1 << allocsize) * PAGE_SIZE;
		ret += sprintf(buf+ret, "%15lld\t\t", allocsize);
		ret += sprintf(buf+ret,
			"%06lld\t\t\t%06lld\n", relalloc, relfree);
	}
	return ret;
}

static struct attribute attr_kmalloc = {
	.name = KMALLOC_FILE_NAME,
	.mode = 0444
};
static struct attribute attr_kzalloc = {
	.name = KZALLOC_FILE_NAME,
	.mode = 0444
};
static struct attribute attr_vmalloc = {
	.name = VMALLOC_FILE_NAME,
	.mode = 0444
};
static struct attribute attr_getfreepages = {
	.name = GETFREEPAGES_FILE_NAME,
	.mode = 0444
};
static struct attribute *d_attrs[] = {
	&attr_kmalloc,
	&attr_kzalloc,
	&attr_vmalloc,
	&attr_getfreepages,
	NULL
};
static const struct sysfs_ops s_ops = {
	.show = show_mem
};
static struct kobj_type k_type = {
	.sysfs_ops = &s_ops,
	.default_attrs = d_attrs
};

static int __init init_mod(void)
{
	mykobj = kzalloc(sizeof(*mykobj), GFP_KERNEL);
	if (mykobj) {
		kobject_init(mykobj, &k_type);
		if (kobject_add(mykobj, NULL, "%s", SYSDIR_NAME)) {
			dev_err(&pdev, "kobject_add() failed\n");
			kobject_put(mykobj);
			mykobj = NULL;
			return -EIO;
		}
	}

	dev_info(&pdev, "module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	if (mykobj) {
		kobject_put(mykobj);
		kfree(mykobj);
	}

	dev_info(&pdev, "module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple sysfs memory management Linux module");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(MYDEV_NAME);
