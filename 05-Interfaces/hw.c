#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

const char *dirName = "my_dir";
const char *entryName = "my_ent1";
const char *statsName = "stats";

static char procFsBuff[PAGE_SIZE];
static ssize_t procFsBuffCntr = 0;

typedef struct {
	int symbInCntr;
	int symbOutCntr;
	int ConvertedCntr;
} statistics_t;

void CreateStatsStr(char *dst, statistics_t *src)
{
	sprintf(dst, "symbInCntr = %d\nsymbOutCntr = %d\nConvertedCntr = %d\n",
			src->symbInCntr, src->symbOutCntr, src->ConvertedCntr);

}

static statistics_t procFsStats = { 0 };

int ConvToUpperCase(char *src, ssize_t count)
{
	int converted = 0;

	for (; count >= 0; count--, src++) {
		if (*src >= 'a' && *src <= 'z') {
			*src = *src - 'a' + 'A';
			converted++;
		}
	}
	return converted;
}

int ConvToLowerCase(char *src, ssize_t count)
{
	int converted = 0;

	for (; count >= 0; count--, src++) {
		if (*src >= 'A' && *src <= 'Z') {
			*src = *src - 'A' + 'a';
			converted++;
		}
	}
	return converted;
}

static ssize_t procFsRead(struct file *file,
		char __user *pbuf,
		size_t count,
		loff_t *ppos)
{
	pr_info("procFsRead");

	ssize_t availableToRead;
	ssize_t nCopied;

	availableToRead = min_t(ssize_t, procFsBuffCntr, count);

	if (availableToRead != 0) {
		nCopied = copy_to_user(pbuf, procFsBuff, availableToRead);
		procFsStats.symbOutCntr += availableToRead - nCopied;
		availableToRead -= nCopied;
		procFsBuffCntr -= availableToRead;
	}

	return availableToRead;
}

static ssize_t procFsWrite(struct file *file,
		const char __user *pbuf,
		size_t count,
		loff_t *ppos)
{
	ssize_t nCopied = 0;

	pr_info("procFsWrite");

	if (count > PAGE_SIZE)
		return -ENOSPC;

	nCopied = copy_from_user(procFsBuff, pbuf, count);

	procFsBuffCntr = count - nCopied;
	procFsStats.symbInCntr += procFsBuffCntr;

	/*converintg*/
	procFsStats.ConvertedCntr += ConvToUpperCase(procFsBuff,
			procFsBuffCntr);

	return procFsBuffCntr;
}

static const struct file_operations mainFile = { .owner = THIS_MODULE, .read =
		procFsRead, .write = procFsWrite, };

static ssize_t procFsStatsRead(struct file *file,
		char __user *pbuf,
		size_t count,
		loff_t *ppos)
{
	static int attamptToRead = 0;

	attamptToRead += 1;
	attamptToRead %= 3;	//magic number - number of attempts cats read
	pr_info("procFsStatsRead");

	char tmp[100] = { 0 };

	CreateStatsStr(tmp, &procFsStats);

	ssize_t nCopied = copy_to_user(pbuf, tmp, ARRAY_SIZE(tmp));

	if (attamptToRead == 1)
		return ARRAY_SIZE(tmp);
	else
		return 0;
}

static const struct file_operations statsFile = { .owner = THIS_MODULE, .read =
		procFsStatsRead, };

const char *sysFsName = "my_nodule";
const char *sysFsEntry = "entry";
const char *sysFsStats_file = "stats";

static char sysFsBuff[PAGE_SIZE];
static statistics_t sysFsStats = { 0 };

static ssize_t rw_show(struct class *class,
		struct class_attribute *attr,
		char *buf)
{
	strcpy(buf, sysFsBuff);

	sysFsStats.symbOutCntr += strlen(sysFsBuff);

	return strlen(sysFsBuff);
}

static ssize_t rw_store(struct class *class,
		struct class_attribute *attr,
		const char *buf,
		size_t count)
{
	strcpy(sysFsBuff, buf);
	sysFsStats.symbInCntr += strlen(sysFsBuff);

	sysFsStats.ConvertedCntr += ConvToLowerCase(sysFsBuff,
			strlen(sysFsBuff));

	return count;
}

/*CLASS_ATTR_RW(rw);*/
struct class_attribute class_attr_rw = { .attr = { .name = "rw", .mode = 0666 },
		.show = rw_show, .store = rw_store };

static ssize_t ro_show(struct class *class,
		struct class_attribute *attr,
		char *buf)
{
	char tmp[512] = {0};

	CreateStatsStr(tmp, &sysFsStats);

	strcpy(buf, tmp);
	return strlen(tmp);
}

/*CLASS_ATTR_RW(ro);*/
struct class_attribute class_attr_ro = { .attr = { .name = "stats",
		.mode = 0444 }, .show = ro_show, };

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent1, *ent2;
static struct class *attr_class = 0;

static int mymodule_init(void)
{
	dir = proc_mkdir(dirName, NULL);
	if (dir == NULL) {
		pr_err("mymodule: error creating procfs directory\n");
		return -ENOMEM;
	}

	ent1 = proc_create(entryName, 0666, dir, &mainFile);
	if (ent1 == NULL) {
		pr_err("mymodule: error creating procfs entry 1\n");
		remove_proc_entry(dirName, NULL);
		return -ENOMEM;
	}

	ent2 = proc_create(statsName, 0444, dir, &statsFile);
	if (ent2 == NULL) {
		pr_err("mymodule: error creating procfs entry 1\n");
		remove_proc_entry(entryName, dir);
		remove_proc_entry(dirName, NULL);
		return -ENOMEM;
	}

	attr_class = class_create(THIS_MODULE, "my_class");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	int ret = class_create_file(attr_class, &class_attr_rw);

	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_ro);
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
	proc_remove(dir);
	class_remove_file(attr_class, &class_attr_rw);
	class_destroy(attr_class);
	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Yuriy Podgorniy");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
