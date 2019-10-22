// SPDX-License-Identifier: GPL
#include <linux/init.h>     // Macros used to mark up functions  __init __exit
#include <linux/module.h>   // Core header for loading LKMs into the kernel
#include <linux/kernel.h>   // Contains types, macros, functions for the kernel

static int myparam;

module_param(myparam, int, 0000);

static int __init hello_init(void)
{
	pr_info("Hello, world!");
	if (myparam >= 0) {
		pr_info("OK, the module is running");
	} else {
		pr_info("Error,the module is not running");
		pr_info("Param it should be >= 0");
	}
	return myparam;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye, world!");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Happybolt <dmytro.topikha@gmail.com>");
MODULE_DESCRIPTION("Simple module for linux kernel");
MODULE_VERSION("0.1");
