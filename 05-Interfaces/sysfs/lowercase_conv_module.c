// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

static int lowercase_converter_module_init(void)
{
	pr_info("lowercase_conv_module: module loading\n");
	pr_info("lowercase_conv_module: module loaded\n");
	return 0;
}

static void lowercase_converter_module_exit(void)
{
	pr_info("lowercase_conv_module: module exited\n");
}

module_init(lowercase_converter_module_init);
module_exit(lowercase_converter_module_exit);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Lowercase converter, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
