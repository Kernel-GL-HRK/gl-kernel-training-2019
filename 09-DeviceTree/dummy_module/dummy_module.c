// SPDX-License-Identifier: GPL-2.0
#include <generated/autoconf.h>
#include <asm-generic/bitsperlong.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/platform_device.h>

static int dummy_probe(struct platform_device *pdev)
{
	pr_info("my dummy driver is loaded!\n");
	return 0;
}

static int dummy_remove(struct platform_device *pdev)
{
	pr_info("my dummy driver is unloaded!\n");
	return 0;
}

static const struct of_device_id dummy_device_table[] = {
	{ .compatible = "orangepi-one,dummy-module" },
	{ }
};

static struct platform_driver my_dummy_driver = {
	.probe = dummy_probe,
	.remove = dummy_remove,
	.driver = {
			.name = "dummy-module",
			.of_match_table = of_match_ptr(dummy_device_table),
			.owner = THIS_MODULE,
	}
};

MODULE_DEVICE_TABLE(of, dummy_device_table);

module_platform_driver(my_dummy_driver);

MODULE_AUTHOR("Vitalii Pudov <pudov@ukr.net>");
MODULE_DESCRIPTION("Dummy module overlay test, Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
