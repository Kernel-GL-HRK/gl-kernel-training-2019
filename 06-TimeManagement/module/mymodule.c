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

#define MYDEV_NAME	"TimeManagement"
#define CLASS_NAME	"my_time_mod"
#define ABSTIME_FILE_NAME	"absolute-time"
#define RELTIME_FILE_NAME	"relation-time"
#define LOADCPU_FILE_NAME	"load-cpu"
#define MS_TO_NS(x) (x * 1E6L)
#define TIMER_DELAY_MS	1000

static struct hrtimer hr_timer;
static struct class *attr_class;
static struct device pdev = { .init_name = MYDEV_NAME };
static u64 oldj64;
static int cpupercent;

enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer)
{
	ktime_t currtime = 0, interval = 0;
	struct file *f = NULL;
	char buf[128] = {0};
	mm_segment_t old_fs;
	char *p = NULL;
	int val = 0, i = 0;
	char *endptr = NULL;
	int cputime = 0, cpuidle = 0;

	f = filp_open("/proc/stat", O_RDONLY, 0);
	if (f == NULL) {
		dev_err(&pdev, "filp_open error\n");
	} else {
		old_fs = get_fs();
		set_fs(get_ds());
		f->f_op->read(f, buf, 128, &f->f_pos);
		set_fs(old_fs);
		filp_close(f, NULL);

		endptr = strstr(buf, "\n");
		*endptr = '\0';
		dev_info(&pdev, "stat: %s\n", buf);

		p = buf;
		p += 5;

		for (i = 0; i < 8; i++) {
			endptr = strstr(p, " ");
			*endptr = '\0';
			val = simple_strtol(p, &endptr, 0);
			p = endptr + 1;
			cputime += val;
			if (i == 3 || i == 4)
				cpuidle += val;
		}

		cpupercent = (cputime - cpuidle) * 100 / cputime;
		dev_info(&pdev, "cpu: %d\n", cpupercent);
	}

	currtime  = ktime_get();
	interval = ktime_set(0, MS_TO_NS(TIMER_DELAY_MS));
	hrtimer_forward(timer, currtime, interval);

	return HRTIMER_RESTART;
}

static ssize_t abstime_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	int ret;
	struct timeval t;

	do_gettimeofday(&t);
	ret = sprintf(buf, "%ld.%06ld\n", t.tv_sec, t.tv_usec);

	return ret;
}

static ssize_t reltime_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	int ret = 0;
	u64 j64 = 0, rel = 0;
	struct timeval t = {};

	j64 = get_jiffies_64();
	rel = j64 - oldj64;
	jiffies_to_timeval(rel, &t);

	dev_info(&pdev, "********************************\n");
	dev_info(&pdev, "jiffies: %ld\n", jiffies);
	dev_info(&pdev, "jiffies to mSec: %d\n", jiffies_to_msecs(jiffies));
	dev_info(&pdev, "jiffies 64 to mSec: %d\n", jiffies_to_msecs(j64));
	dev_info(&pdev, "jiffies 64: %lld\n", j64);
	dev_info(&pdev, "jiffies 64 old: %lld\n", oldj64);
	dev_info(&pdev, "jiffies 64 rel: %lld\n", rel);
	dev_info(&pdev, "%ld.%06ld\n", t.tv_sec, t.tv_usec);

	ret = sprintf(buf, "%ld.%06ld\n", t.tv_sec, t.tv_usec);

	oldj64 = j64;

	return ret;
}

static ssize_t loadcpu_show(struct class *class, struct class_attribute *attr,
	char *buf)
{
	return sprintf(buf, "%d\n", cpupercent);
}

struct class_attribute class_attr_abstime = {
	.attr = { .name = ABSTIME_FILE_NAME, .mode = 0444 },
	.show = abstime_show,
};

struct class_attribute class_attr_reltime = {
	.attr = { .name = RELTIME_FILE_NAME, .mode = 0444 },
	.show = reltime_show,
};

struct class_attribute class_attr_loadcpu = {
	.attr = { .name = LOADCPU_FILE_NAME, .mode = 0444 },
	.show = loadcpu_show,
};

static int __init init_mod(void)
{
	int ret;
	ktime_t ktime;

	attr_class = class_create(THIS_MODULE, CLASS_NAME);
	if (attr_class == NULL) {
		dev_err(&pdev, "error creating sysfs class: %s\n",
			CLASS_NAME);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_abstime);
	if (ret) {
		dev_err(&pdev, "error creating sysfs class attribute %s\n",
			ABSTIME_FILE_NAME);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_reltime);
	if (ret) {
		dev_err(&pdev, "error creating sysfs class attribute %s\n",
			RELTIME_FILE_NAME);
		class_remove_file(attr_class, &class_attr_abstime);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_loadcpu);
	if (ret) {
		dev_err(&pdev, "error creating sysfs class attribute %s\n",
			LOADCPU_FILE_NAME);
		class_remove_file(attr_class, &class_attr_reltime);
		class_remove_file(attr_class, &class_attr_abstime);
		class_destroy(attr_class);
		return ret;
	}

	dev_info(&pdev, "HR Timer module installing\n");
	ktime = ktime_set(0, MS_TO_NS(TIMER_DELAY_MS));
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &my_hrtimer_callback;
	dev_info(&pdev, "Starting timer to fire in %dms (%lu)\n",
		1000, jiffies);
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);

	dev_info(&pdev, "module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	int ret;

	class_remove_file(attr_class, &class_attr_abstime);
	class_remove_file(attr_class, &class_attr_reltime);
	class_remove_file(attr_class, &class_attr_loadcpu);
	class_destroy(attr_class);

	ret = hrtimer_cancel(&hr_timer);
	if (ret)
		dev_err(&pdev, "The timer was still in use...\n");

	dev_info(&pdev, "module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple sysfs time management Linux module");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(MYDEV_NAME);
