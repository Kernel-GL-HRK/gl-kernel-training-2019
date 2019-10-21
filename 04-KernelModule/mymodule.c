// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kernel.h>

static int myparam = -1;
module_param(myparam, int, 0);

static int __init mymodule_init(void)
{
	pr_info("mymodule: myparam = %d\n", myparam);
	if (myparam < 0)
		return myparam;

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void __exit mymodule_exit(void)
{
	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Lapin Vadim <lapin.vadim@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");