#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static int auto_loadable_module_probe(struct platform_device *pdev)
{
	pr_info("auto loadable module: module loaded\n");
	return 0;
}

static int auto_loadable_module_remove(struct platform_device *pdev)
{
	pr_info("auto loadable module: module exited\n");
	return 0;
}

static const struct of_device_id auto_loadable_module_ids[] = {
	{ .compatible = "auto_loadable_module" },
	{  },
};

MODULE_DEVICE_TABLE(of, auto_loadable_module_ids);

static struct platform_driver auto_loadable_module = {
	.probe = auto_loadable_module_probe,
	.remove = auto_loadable_module_remove,
	.driver = {
		.name = "my_auto_loadable_module",
		.of_match_table = of_match_ptr(auto_loadable_module_ids),
		.owner = THIS_MODULE,
	}
};

module_platform_driver(auto_loadable_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Degtyarov");
MODULE_DESCRIPTION("Kernel module for auto loading");
MODULE_VERSION("0.1");
