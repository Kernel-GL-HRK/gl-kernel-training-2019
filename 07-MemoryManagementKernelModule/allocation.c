#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

void bubbleSort(long arr[], int n)
{
	int i, j;

	for (i = 0; i < n-1; i++) {
		for (j = 0; j < n-i-1; j++) {
			if (arr[j] > arr[j+1])
				swap(arr[j], arr[j+1]);
		}
	}
}

struct timespec sub(struct timespec arg1, struct timespec arg2)
{
	struct timespec ret;
	ret.tv_sec = 0;
	ret.tv_nsec = 0;

	// pr_info("arg1 = %ld.%09ld", arg1.tv_sec, arg1.tv_nsec);
	// pr_info("arg2 = %ld.%09ld", arg2.tv_sec, arg2.tv_nsec);

	ret.tv_sec = arg1.tv_sec - arg2.tv_sec;
	ret.tv_nsec = arg1.tv_nsec - arg2.tv_nsec;
	if (ret.tv_nsec < 0) {
		ret.tv_nsec += 1000000000;
		ret.tv_sec -= 1;
	}

	// pr_info("ret = %ld.%09ld", ret.tv_sec, ret.tv_nsec);

	return ret;
}

static void Allocate(void)
{
	void *ptr;

	pr_info("time in ns");

	pr_info("kmalloc:size	minAlloc	avrAlloc	maxAlloc	minFree		avrFree		maxFree");

	int power = 0;
	int oversize = 0;

	while (power < 65) {
		const int attempts = 9;
		struct timespec t1, t2;
		struct timespec deltaAlloc[attempts], deltaFree[attempts];
		unsigned long long size;

		int i = 0;

		for (; i < attempts; i++) {
			size = (1 << power) + oversize;
			getnstimeofday(&t1);
			ptr = kmalloc(size, GFP_KERNEL);
			getnstimeofday(&t2);
			deltaAlloc[i] = sub(t2, t1);

			getnstimeofday(&t1);
			kfree(ptr);
			getnstimeofday(&t2);
			deltaFree[i] = sub(t2, t1);
		}

		long avrAllocNS = 0, avrFreeNS = 0;
		long minAllocNS = 0, minFreeNS = 0;
		long maxAllocNS = 0, maxFreeNS = 0;
		long deltaAllocNS[attempts], deltaFreeNS[attempts];

		i = 0;
		for (; i < attempts; i++) {
			deltaAllocNS[i] = deltaAlloc[i].tv_sec * 1000000000;
			deltaAllocNS[i] += deltaAlloc[i].tv_nsec;
			deltaFreeNS[i] = deltaFree[i].tv_sec * 1000000000;
			deltaFreeNS[i] += deltaFree[i].tv_nsec;
		}

		minAllocNS = maxAllocNS = deltaAllocNS[0];
		minFreeNS = maxFreeNS = deltaFreeNS[0];
		i = 0;
		for (; i < attempts; i++) {
			avrAllocNS += deltaAllocNS[i];
			avrFreeNS += deltaFreeNS[i];
			minAllocNS = min(minAllocNS, deltaAllocNS[i]);
			minFreeNS = min(minFreeNS, deltaFreeNS[i]);
			maxAllocNS = max(maxAllocNS, deltaAllocNS[i]);
			maxFreeNS = max(maxFreeNS, deltaFreeNS[i]);
		}

		avrAllocNS /= attempts;
		avrFreeNS /= attempts;

		pr_info("kmalloc:2^%02d+%d	%09ld	%09ld	%09ld	%09ld	%09ld	%09ld",
			power, oversize, minAllocNS, avrAllocNS, maxAllocNS,
			minFreeNS, avrFreeNS, maxFreeNS);

		oversize++;
		oversize %= 2;
		if (oversize == 0)
			power++;
	}

	power = 0;
	oversize = 0;

	pr_info("kzalloc:size	minAlloc	avrAlloc	maxAlloc	minFree		avrFree		maxFree");

	while (power < 65) {
		const int attempts = 9;
		struct timespec t1, t2;
		struct timespec deltaAlloc[attempts], deltaFree[attempts];
		unsigned long long size;

		int i = 0;

		for (; i < attempts; i++) {
			size = (1 << power) + oversize;
			getnstimeofday(&t1);
			ptr = kzalloc(size, GFP_KERNEL);
			getnstimeofday(&t2);
			deltaAlloc[i] = sub(t2, t1);

			getnstimeofday(&t1);
			kfree(ptr);
			getnstimeofday(&t2);
			deltaFree[i] = sub(t2, t1);
		}

		long avrAllocNS = 0, avrFreeNS = 0;
		long minAllocNS = 0, minFreeNS = 0;
		long maxAllocNS = 0, maxFreeNS = 0;
		long deltaAllocNS[attempts], deltaFreeNS[attempts];

		i = 0;
		for (; i < attempts; i++) {
			deltaAllocNS[i] = deltaAlloc[i].tv_sec * 1000000000;
			deltaAllocNS[i] += deltaAlloc[i].tv_nsec;
			deltaFreeNS[i] = deltaFree[i].tv_sec * 1000000000;
			deltaFreeNS[i] += deltaFree[i].tv_nsec;
		}

		minAllocNS = maxAllocNS = deltaAllocNS[0];
		minFreeNS = maxFreeNS = deltaFreeNS[0];
		i = 0;
		for (; i < attempts; i++) {
			avrAllocNS += deltaAllocNS[i];
			avrFreeNS += deltaFreeNS[i];
			minAllocNS = min(minAllocNS, deltaAllocNS[i]);
			minFreeNS = min(minFreeNS, deltaFreeNS[i]);
			maxAllocNS = max(maxAllocNS, deltaAllocNS[i]);
			maxFreeNS = max(maxFreeNS, deltaFreeNS[i]);
		}

		avrAllocNS /= attempts;
		avrFreeNS /= attempts;

		pr_info("kzalloc:2^%02d+%d	%09ld	%09ld	%09ld	%09ld	%09ld	%09ld",
			power, oversize, minAllocNS, avrAllocNS, maxAllocNS,
			minFreeNS, avrFreeNS, maxFreeNS);

		oversize++;
		oversize %= 2;
		if (oversize == 0)
			power++;
	}

	power = 0;
	oversize = 0;

	pr_info("vmalloc:size	minAlloc	avrAlloc	maxAlloc	minFree		avrFree		maxFree");
	while (power < 65) {
		const int attempts = 9;
		struct timespec t1, t2;
		struct timespec  deltaAlloc[attempts], deltaFree[attempts];
		unsigned long long size;

		int i = 0;

		for (; i < attempts; i++) {
			size = (1 << power) + oversize;
			getnstimeofday(&t1);
			ptr = vmalloc(size);
			getnstimeofday(&t2);
			deltaAlloc[i] = sub(t2, t1);

			getnstimeofday(&t1);
			vfree(ptr);
			getnstimeofday(&t2);
			deltaFree[i] = sub(t2, t1);
		}

		long avrAllocNS = 0, avrFreeNS = 0;
		long minAllocNS = 0, minFreeNS = 0;
		long maxAllocNS = 0, maxFreeNS = 0;
		long deltaAllocNS[attempts], deltaFreeNS[attempts];

		i = 0;
		for (; i < attempts; i++) {
			deltaAllocNS[i] = deltaAlloc[i].tv_sec * 1000000000;
			deltaAllocNS[i] += deltaAlloc[i].tv_nsec;
			deltaFreeNS[i] = deltaFree[i].tv_sec * 1000000000;
			deltaFreeNS[i] += deltaFree[i].tv_nsec;
		}

		minAllocNS = maxAllocNS = deltaAllocNS[0];
		minFreeNS = maxFreeNS = deltaFreeNS[0];
		i = 0;
		for (; i < attempts; i++) {
			avrAllocNS += deltaAllocNS[i];
			avrFreeNS += deltaFreeNS[i];
			minAllocNS = min(minAllocNS, deltaAllocNS[i]);
			minFreeNS = min(minFreeNS, deltaFreeNS[i]);
			maxAllocNS = max(maxAllocNS, deltaAllocNS[i]);
			maxFreeNS = max(maxFreeNS, deltaFreeNS[i]);
		}

		avrAllocNS /= attempts;
		avrFreeNS /= attempts;

		pr_info("vmalloc:2^%02d+%d	%09ld	%09ld	%09ld	%09ld	%09ld	%09ld",
			power, oversize, minAllocNS, avrAllocNS, maxAllocNS,
			minFreeNS, avrFreeNS, maxFreeNS);

		oversize++;
		oversize %= 2;
		if (oversize == 0)
			power++;
	}

	power = 0;
	oversize = 0;

	pr_info("GDP:pageNum	minAlloc	avrAlloc	maxAlloc	minFree		avrFree		maxFree");
	while (power < 65) {
		const int attempts = 9;
		struct timespec t1, t2;
		struct timespec deltaAlloc[attempts], deltaFree[attempts];

		int i = 0;

		for (; i < attempts; i++) {
			getnstimeofday(&t1);
			ptr = (void *) __get_free_pages(GFP_KERNEL, power);
			if (ptr == NULL)
				break;
			getnstimeofday(&t2);
			deltaAlloc[i] = sub(t2, t1);

			getnstimeofday(&t1);
			free_pages((unsigned long)ptr, power);
			getnstimeofday(&t2);
			deltaFree[i] = sub(t2, t1);
		}

		long avrAllocNS = 0, avrFreeNS = 0;
		long minAllocNS = 0, minFreeNS = 0;
		long maxAllocNS = 0, maxFreeNS = 0;
		long deltaAllocNS[attempts], deltaFreeNS[attempts];

		i = 0;
		for (; i < attempts; i++) {
			deltaAllocNS[i] = deltaAlloc[i].tv_sec * 1000000000;
			deltaAllocNS[i] += deltaAlloc[i].tv_nsec;
			deltaFreeNS[i] = deltaFree[i].tv_sec * 1000000000;
			deltaFreeNS[i] += deltaFree[i].tv_nsec;
		}

		minAllocNS = maxAllocNS = deltaAllocNS[0];
		minFreeNS = maxFreeNS = deltaFreeNS[0];
		i = 0;
		for (; i < attempts; i++) {
			avrAllocNS += deltaAllocNS[i];
			avrFreeNS += deltaFreeNS[i];
			minAllocNS = min(minAllocNS, deltaAllocNS[i]);
			minFreeNS = min(minFreeNS, deltaFreeNS[i]);
			maxAllocNS = max(maxAllocNS, deltaAllocNS[i]);
			maxFreeNS = max(maxFreeNS, deltaFreeNS[i]);
		}

		avrAllocNS /= attempts;
		avrFreeNS /= attempts;

		pr_info("GFP:2^%02d	%09ld	%09ld	%09ld	%09ld	%09ld	%09ld",
			power, minAllocNS, avrAllocNS, maxAllocNS,
			minFreeNS, avrFreeNS, maxFreeNS);

		power++;
	}
}

static struct class *attr_class;

static int mymodule_init(void)
{
	Allocate();
	return -1;
}

// static void mymodule_exit(void)
// {
//	class_remove_file(attr_class, &class_attr_ro);
//	class_destroy(attr_class);
//	pr_info("mymodule: module exited\n");
// }

module_init(mymodule_init);
// module_exit(mymodule_exit);

MODULE_AUTHOR("Yuriy Podgorniy");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
