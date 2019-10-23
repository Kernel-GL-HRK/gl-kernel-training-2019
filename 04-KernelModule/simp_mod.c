/*
 * simp_mod.c for Hello World
 *
 * Copyright (C) 2019 Chekanov. All Rights reserved.
 * This program is free software.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kernel.h>

static int myparam;
module_param(myparam, int, 0);

static int __init hello_init(void)
{
	pr_info("hello: Hello, world!\n");
	pr_info("hello: module loading\n");
	pr_info("hello: myparam = %d\n", myparam);

	return myparam;
}

static void __exit hello_exit(void)
{
	pr_info("hello: module shutdown\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Serhii Chekanov <chekanovserhii@gmail.com>");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

