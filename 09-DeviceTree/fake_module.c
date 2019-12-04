#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>	/* For platform devices */
#include <linux/of.h>		/* For DT*/
#include <linux/device.h>

#define MYDEV_NAME	"FakeModuleDt"

/*----------------------------------------------------------------------------*/
static int fake_module_probe(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Module started\n");

	return 0;
}
/*----------------------------------------------------------------------------*/
static int fake_module_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Module removed\n");

	return 0;
}
/*----------------------------------------------------------------------------*/
static const struct of_device_id fake_module_ids[] = {
	{ .compatible = "orangepi-one,fake-module" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, fake_module_ids);
/*----------------------------------------------------------------------------*/
static struct platform_driver fake_module = {
	.probe = fake_module_probe,
	.remove = fake_module_remove,
	.driver = {
		.name = "fake-module-dt",
		.of_match_table = of_match_ptr(fake_module_ids),
		.owner = THIS_MODULE,
	},
};
module_platform_driver(fake_module);
/*----------------------------------------------------------------------------*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Domnin Dmitry");
MODULE_DESCRIPTION("A simple fake module, which is start from device tree");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(MYDEV_NAME);
/*----------------------------------------------------------------------------*/
