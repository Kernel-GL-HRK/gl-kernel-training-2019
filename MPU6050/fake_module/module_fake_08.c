// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  module_fake_08.c - Test module
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/err.h>

#define NAME "module_fake_08"

static int module_fake_08_probe(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "%s:%d\n", __func__, __LINE__);
	return 0;
}

static int module_fake_08_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "%s:%d\n", __func__, __LINE__);
	return 0;
}

static struct platform_device_id tbl[] = {
	{"module_fake0"},
	{},
};

MODULE_DEVICE_TABLE(platform, tbl);

#ifdef CONFIG_OF
static const struct of_device_id of_tbl[] = {
	{.compatible = "module_fake0",},
	{},
};

MODULE_DEVICE_TABLE(of, of_tbl);
#endif


static struct platform_driver drv = {
	.probe  = module_fake_08_probe,
	.remove = module_fake_08_remove,

	.driver = {
		.name = NAME,
#ifdef CONFIG_OF
		.of_match_table = of_tbl,
#endif
		.owner = THIS_MODULE,
	},

	.id_table = tbl,
};


static int __init module_fake_08_init(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	return platform_driver_register(&drv);
}

static void __exit module_fake_08_exit(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	platform_driver_unregister(&drv);
}

module_init(module_fake_08_init);
module_exit(module_fake_08_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("A simple Kernel module.");
MODULE_VERSION("0.1");
