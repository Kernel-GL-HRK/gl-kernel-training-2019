// SPDX-License-Identifier: GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/err.h>

static int fake_module_probe(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "loaded\n");
	return 0;
}

static int fake_module_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "removed\n");
	return 0;
}

static const struct of_device_id fake_module_ids[] = {
	{ .compatible = "fake-module" },
	{  },
};

MODULE_DEVICE_TABLE(of, fake_module_ids);

static struct platform_driver fake_module = {
	.probe = fake_module_probe,
	.remove = fake_module_remove,
	.driver = {
		.name = "fake_module_ovr",
		.of_match_table = of_match_ptr(fake_module_ids),
		.owner = THIS_MODULE,
	},
};
module_platform_driver(fake_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Topikha");
MODULE_DESCRIPTION("Fake module, for test dtsi");
MODULE_VERSION("0.1");
