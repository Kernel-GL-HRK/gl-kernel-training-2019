#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/slab.h>

char *pArr;

enum ALLOC_TYPE_e {
	TYPE_KMALLOC = 0,
	TYPE_KZMALLOC = 1,
	TYPE_VMALLOC = 2,
	TYPE_GFP = 3
};

struct outResult_t {
	unsigned int allocAvrTime;
	unsigned int allocMinTime;
	unsigned int allocMaxTime;
	unsigned int freeAvrTime;
	unsigned int freeMinTime;
	unsigned int freeMaxTime;
};

unsigned int calcNsTimeDiff(struct timespec *tStart, struct timespec *tStop)
{
	return 1000000000 * (tStop->tv_sec - tStart->tv_sec) +
		(tStop->tv_nsec - tStart->tv_nsec);
}

int getTimeMemoryAllocation(void *pArr, unsigned int size_order, int type,
				 struct outResult_t *pResult)
{
	struct timespec tStart, tStop;

	unsigned int allocTimeSum = 0;
	unsigned int freeTimeSum = 0;

	int allocation_error = 0;

	pResult->allocMinTime = 999999999;
	pResult->allocMaxTime = 0;
	pResult->freeMinTime = 999999999;
	pResult->freeMaxTime = 0;

	int i;

	for (i = 0; i < 5; i++) {
		switch (type) {
		case TYPE_KMALLOC:
			getnstimeofday(&tStart);
			pArr = kmalloc(size_order, GFP_KERNEL);
			getnstimeofday(&tStop);
			break;
		case TYPE_KZMALLOC:
			getnstimeofday(&tStart);
			pArr =  kzalloc(size_order, GFP_KERNEL);
			getnstimeofday(&tStop);
			break;
		case TYPE_VMALLOC:
			getnstimeofday(&tStart);
			pArr = vmalloc(size_order);
			getnstimeofday(&tStop);
			break;
		case TYPE_GFP:
			getnstimeofday(&tStart);
			pArr = (char *)__get_free_pages(GFP_KERNEL, size_order);
			getnstimeofday(&tStop);
			break;
		}

		if (pArr != NULL) {
			unsigned int currTimeDiff =
				calcNsTimeDiff(&tStart, &tStop);

			allocTimeSum += currTimeDiff;
			if (currTimeDiff <= pResult->allocMinTime)
				pResult->allocMinTime = currTimeDiff;
			if (currTimeDiff >= pResult->allocMaxTime)
				pResult->allocMaxTime = currTimeDiff;

			// free
			switch (type) {
			case TYPE_KMALLOC:
				getnstimeofday(&tStart);
				kfree(pArr);
				getnstimeofday(&tStop);
				break;
			case TYPE_KZMALLOC:
				getnstimeofday(&tStart);
				kzfree(pArr);
				getnstimeofday(&tStop);
				break;
			case TYPE_VMALLOC:
				getnstimeofday(&tStart);
				vfree(pArr);
				getnstimeofday(&tStop);
				break;
			case TYPE_GFP:
				getnstimeofday(&tStart);
				free_pages((unsigned long)pArr, size_order);
				getnstimeofday(&tStop);
				break;
			}

			currTimeDiff = calcNsTimeDiff(&tStart, &tStop);

			freeTimeSum += currTimeDiff;
			if (currTimeDiff <= pResult->freeMinTime)
				pResult->freeMinTime = currTimeDiff;
			if (currTimeDiff >= pResult->freeMaxTime)
				pResult->freeMaxTime = currTimeDiff;
		} else {
			allocation_error = 1;
			break;
		}
	}

	if (allocation_error == 0) {
		pResult->allocAvrTime = allocTimeSum/5;
		pResult->freeAvrTime = freeTimeSum/5;
		return 1;
	} else
		return 0;
}

void showTestMemoryAllocationTable(char *buf, int type, int range)
{
	char tmpBuff[200];

	strcat(buf, "---------------------------------------------");
	strcat(buf, "-------------------------------------------\n");

	strcat(buf, "|   bufer size | alloc min | alloc avr | alloc max ");
	strcat(buf, "|  free min |  free avr |  free max |\n");

	strcat(buf, "|        bytes |        ns |        ns |        ns ");
	strcat(buf, "|        ns |        ns |        ns |\n");

	strcat(buf, "---------------------------------------------");
	strcat(buf, "-------------------------------------------\n");

	struct outResult_t result;

	unsigned int shift = range * 16;
	int allocation_error = 0;
	unsigned int i, j;

	for (i = 0 + shift; i < 16 + shift; i++) {
		for (j = 0; j < 2; j++) {
			unsigned int size_order;

			if (type != TYPE_GFP)
				size_order = (1 << i) + j;
			else
				size_order = i;

			if (getTimeMemoryAllocation(pArr, size_order, type,
				&result)) {
				sprintf(tmpBuff, "|%10u + %u|%10u |%10u ",
					(1 << i), j, result.allocMinTime,
					result.allocAvrTime);
				strcat(buf, tmpBuff);

				sprintf(tmpBuff, "|%10u |%10u |%10u |%10u |\n",
					result.allocMaxTime,
					result.freeMinTime,
					result.freeAvrTime,
					result.freeMaxTime);
				strcat(buf, tmpBuff);
			} else {
				sprintf(tmpBuff, "Allocation error, buff size");
				strcat(buf, tmpBuff);

				sprintf(tmpBuff, "= %u + %u\n", (1 << i), j);
				strcat(buf, tmpBuff);

				allocation_error = 1;
				break;
			}

			if (type == TYPE_GFP)
				break;
		}

		// check allocation error
		if (allocation_error)
			break;
	}

	strcat(buf, "---------------------------------------------");
	strcat(buf, "-------------------------------------------\n");
}

static ssize_t kmallocstat1_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_KMALLOC, 0);

	return strlen(buf);
}

static ssize_t kmallocstat2_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_KMALLOC, 1);

	return strlen(buf);
}

static ssize_t kzmallocstat1_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_KZMALLOC, 0);

	return strlen(buf);
}

static ssize_t kzmallocstat2_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_KZMALLOC, 1);

	return strlen(buf);
}

static ssize_t vmallocstat1_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_VMALLOC, 0);

	return strlen(buf);
}

static ssize_t vmallocstat2_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_VMALLOC, 1);

	return strlen(buf);
}

static ssize_t gfpstat_show(struct class *class,
	struct class_attribute *attr, char *buf)
{
	showTestMemoryAllocationTable(buf, TYPE_GFP, 0);

	return strlen(buf);
}

struct class_attribute class_attr_kmallocstat1 = {
	.attr = { .name = "kmalloc_stat1", .mode = 0666 },
	.show	= kmallocstat1_show,
};

struct class_attribute class_attr_kmallocstat2 = {
	.attr = { .name = "kmalloc_stat2", .mode = 0666 },
	.show	= kmallocstat2_show,
};

struct class_attribute class_attr_kzmallocstat1 = {
	.attr = { .name = "kzmalloc_stat1", .mode = 0666 },
	.show	= kzmallocstat1_show,
};

struct class_attribute class_attr_kzmallocstat2 = {
	.attr = { .name = "kzmalloc_stat2", .mode = 0666 },
	.show	= kzmallocstat2_show,
};

struct class_attribute class_attr_vmallocstat1 = {
	.attr = { .name = "vmalloc_stat1", .mode = 0666 },
	.show	= vmallocstat1_show,
};

struct class_attribute class_attr_vmallocstat2 = {
	.attr = { .name = "vmalloc_stat2", .mode = 0666 },
	.show	= vmallocstat2_show,
};

struct class_attribute class_attr_gfpstat = {
	.attr = { .name = "getFreePages_stat", .mode = 0666 },
	.show	= gfpstat_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "mem_allocation_stat");
	if (attr_class == NULL) {
		pr_err("mem_alloc_module: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_kmallocstat1);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_kmallocstat2);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_kmallocstat1);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_kzmallocstat1);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_kmallocstat2);
		class_remove_file(attr_class, &class_attr_kmallocstat1);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_kzmallocstat2);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_kzmallocstat1);
		class_remove_file(attr_class, &class_attr_kmallocstat2);
		class_remove_file(attr_class, &class_attr_kmallocstat1);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_vmallocstat1);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_kzmallocstat2);
		class_remove_file(attr_class, &class_attr_kzmallocstat1);
		class_remove_file(attr_class, &class_attr_kmallocstat2);
		class_remove_file(attr_class, &class_attr_kmallocstat1);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_vmallocstat2);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_vmallocstat1);
		class_remove_file(attr_class, &class_attr_kzmallocstat2);
		class_remove_file(attr_class, &class_attr_kzmallocstat1);
		class_remove_file(attr_class, &class_attr_kmallocstat2);
		class_remove_file(attr_class, &class_attr_kmallocstat1);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_gfpstat);
	if (ret) {
		pr_err("mem_alloc_module: error sysfs class attribute\n");
		class_remove_file(attr_class, &class_attr_vmallocstat2);
		class_remove_file(attr_class, &class_attr_vmallocstat1);
		class_remove_file(attr_class, &class_attr_kzmallocstat2);
		class_remove_file(attr_class, &class_attr_kzmallocstat1);
		class_remove_file(attr_class, &class_attr_kmallocstat2);
		class_remove_file(attr_class, &class_attr_kmallocstat1);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mem_alloc_module: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_gfpstat);
	class_remove_file(attr_class, &class_attr_vmallocstat2);
	class_remove_file(attr_class, &class_attr_vmallocstat1);
	class_remove_file(attr_class, &class_attr_kzmallocstat2);
	class_remove_file(attr_class, &class_attr_kzmallocstat1);
	class_remove_file(attr_class, &class_attr_kmallocstat2);
	class_remove_file(attr_class, &class_attr_kmallocstat1);
	class_destroy(attr_class);

	pr_info("mem_alloc_module: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton");
MODULE_DESCRIPTION("Memory allocation kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
