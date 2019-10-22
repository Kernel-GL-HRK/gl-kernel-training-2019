// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>

static int vp_param = -1;

module_param(vp_param, int, 0);

static int vp_module_init(void)
{
	pr_info("vp_module: vp_param = %d\n", vp_param);
	pr_info("vp_module loaded\n");
	return vp_param;
}

static void vp_module_exit(void)
{
	pr_info("vp_module: module exited");
}

module_init(vp_module_init);
module_exit(vp_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Test Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
