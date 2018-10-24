// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  module_04.c - Test module
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int param = -1;
module_param(param, int, 0);

static int __init module_04(void)
{
	pr_info("%s: module starting\n",  __func__);

	pr_info("%s: param = %d\n", __func__, param);
	return param;
}

static void __exit module_04_exit(void)
{
	pr_info("%s: module exit\n",  __func__);
}


module_init(module_04);
module_exit(module_04_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("A simple Kernel module.");
MODULE_VERSION("0.1");
