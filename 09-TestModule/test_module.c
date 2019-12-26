// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/i2c.h>


#define DBG_INFO(x, arg...) \
pr_info("My_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("My_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)


static int test_module_probe(struct i2c_client *client,
						const struct i2c_device_id *id)
{
	struct device_node *dn = client->dev.of_node;
	const char *status;

	status = of_get_property(dn, "status", NULL);
	DBG_INFO("status = %s\n", status);
	DBG_INFO("module loaded\n");
	return 0;
}

static int test_module_remove(struct i2c_client *client)
{
	DBG_INFO("module removed\n");
	return 0;
}

static const struct of_device_id test_module_ids[] = {
	{ .compatible = "module_test0" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, test_module_ids);

static struct i2c_driver test_module_struct = {
	.probe = test_module_probe,
	.remove = test_module_remove,
	.driver = {
		.name = "test_module_device_tree",
		.of_match_table = of_match_ptr(test_module_ids),
		.owner = THIS_MODULE,
	},
};

module_i2c_driver(test_module_struct);

MODULE_AUTHOR("Lapin Vadim <lapin.vadim@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
