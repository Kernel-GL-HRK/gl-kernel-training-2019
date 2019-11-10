// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  module_06.c - Test module
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/sched/loadavg.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/sched/stat.h>
#include <linux/cpumask.h>
#include <linux/math64.h>
#include <linux/kernel_stat.h>
#include <linux/tick.h>


static struct kobject *kobject_module_06;

static struct hrtimer hr_timer;
static ktime_t ktime;


struct Tcpu_procent//100.00 %  H.L %
{
	u64 H;
	u64 L;
};

static struct Tcpu_procent cpu_procent;


#define INTERVAL_1NS 1000000000ULL
#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) LOAD_INT(((x) & (FIXED_1-1)) * 100)

static u64 get_idle_time(int cpu)
{
	u64 idle, idle_usecs = -1ULL;

	if (cpu_online(cpu))
		idle_usecs = get_cpu_idle_time_us(cpu, NULL);


	if (idle_usecs == -1ULL)
		/* !NO_HZ or cpu offline so we can rely on cpustat.idle */
		idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	else
		idle = idle_usecs * NSEC_PER_USEC;

	return idle;
}


static ssize_t real_loadavg_show(struct kobject *kobj,
	struct kobj_attribute *attr, char *buf)
{

	return sprintf(buf, "CPU: %llu.%02llu %%\n",
		cpu_procent.H, cpu_procent.L);
}


static ssize_t absolute_time_show(struct kobject *kobj,
	struct kobj_attribute *attr,
		      char *buf)
{
	struct timespec timespec_now;
	u64 ticks_now, ticks_temp;
	static u64 ticks_old;
	u64 rem;

	getnstimeofday(&timespec_now);

	ticks_now = (timespec_now.tv_sec * INTERVAL_1NS + timespec_now.tv_nsec);

	ticks_temp = ticks_now - ticks_old;
	ticks_temp = div64_u64_rem(ticks_temp, INTERVAL_1NS, &rem);

	ticks_old = ticks_now;

	return sprintf(buf, "dt: %llu.%09llu s\n", ticks_temp, rem);
}

static ssize_t relation_time_show(struct kobject *kobj,
	struct kobj_attribute *attr,
		      char *buf)
{
	u64 ticks_now, ticks_temp;
	static u64 ticks_old;
	u64 rem;

	ticks_now = get_jiffies_64();

	ticks_temp = ticks_now - ticks_old;
	ticks_temp = div64_u64_rem(ticks_temp, HZ, &rem);

	ticks_old = ticks_now;
	return sprintf(buf, "dt: %llu.%03llu s\n", ticks_temp, rem);
}


static enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
	int i;
	u64 curent_ticks;
	static u64 old_ticks;
	ktime_t currtime, interval;
	u64 time_idle, time_system;
	static u64 old_idle, old_system;
	u64 a, b, c, real, rem;

	currtime  = ktime_get();

	interval = ktime_set(0, INTERVAL_1NS);
	hrtimer_forward(timer_for_restart, currtime, interval);

	curent_ticks = get_jiffies_64();

	pr_info("delta jiffies: %09llu",  curent_ticks-old_ticks);
	old_ticks = curent_ticks;

	time_system = ktime_get_with_offset(TK_OFFS_BOOT);

	for_each_online_cpu(i) {
		time_idle +=  get_idle_time(i);
	}

	a = (time_idle - old_idle)*10000;
	b = time_system - old_system;
	c = 10000 - div64_u64(a, b);

	real = div64_u64_rem(c, 100, &rem);

	cpu_procent.H = real;
	cpu_procent.L = rem;

	pr_info("CPU: %03llu.%02llu %%\n",  cpu_procent.H, cpu_procent.L);

	old_idle = time_idle;
	old_system = time_system;

	return HRTIMER_RESTART;
}

static struct kobj_attribute absolute_time_attribute =
	__ATTR(absolute_time, 0444, absolute_time_show, NULL);

static struct kobj_attribute relation_time_attribute =
	__ATTR(relation_time, 0444, relation_time_show, NULL);

static struct kobj_attribute real_loadavg_attribute =
	__ATTR(real_loadavg, 0444, real_loadavg_show, NULL);

static struct attribute *attrs[] = {
	&relation_time_attribute.attr,
	&absolute_time_attribute.attr,
	&real_loadavg_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};


static int module_06(void)
{
	int retval;

	pr_info("%s: module starting\n",  __func__);

	// /sys/module_06/
	 kobject_module_06 = kobject_create_and_add("module_06", NULL);
	if (!kobject_module_06)
		return -ENOMEM;

	retval = sysfs_create_group(kobject_module_06, &attr_group);
	if (retval)
		kobject_put(kobject_module_06);


	ktime = ktime_set(0, INTERVAL_1NS);
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &timer_callback;
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);

	pr_info("%s: module start: %d\n",  __func__, retval);
	return retval;
}

static void module_06_exit(void)
{
	sysfs_remove_group(kobject_module_06, &attr_group);
	kobject_put(kobject_module_06);
	hrtimer_cancel(&hr_timer);
	pr_info("%s: module exit\n",  __func__);

}


module_init(module_06);
module_exit(module_06_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("A simple Kernel module.");
MODULE_VERSION("0.1");
