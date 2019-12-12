#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>	
#include <linux/of.h>	
#include <linux/device.h>

#define TEST_DTS_AND_MODULE	"test_module_mpu6050"
#define NAME_WHO "Vladyslav Kryzhanovskyi"

static int test_module_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Module removed\n");
	return 0;
}

static int test_module_probe(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Module started\n");
	return 0;
}
static const struct of_device_id test_module_ids[] = {
	{ .compatible = "orangepi_one,test_module_for_mpu6050" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, test_module_ids);

static struct platform_driver test_module_mpu6050_struct = {
	.probe = test_module_probe,
	.remove = test_module_remove,
	.driver = {
		.name = "test_module_dts",
		.of_match_table = of_match_ptr(test_module_ids),
		.owner = THIS_MODULE,
	},
};
module_platform_driver(test_module_mpu6050_struct);

MODULE_AUTHOR("NAME_WHO");
MODULE_DESCRIPTION("Use this test module for test dts.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(TEST_DTS_AND_MODULE);