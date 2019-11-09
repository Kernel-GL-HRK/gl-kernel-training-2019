#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

#define MYDEV_NAME "TimeManagement"

static struct device pdev = { .init_name = MYDEV_NAME };

static int __init init_mod(void)
{
	dev_info(&pdev, "module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	dev_info(&pdev, "module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple sysfs time management Linux module");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(MYDEV_NAME);