#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>

static int myparam;
module_param(myparam, int, 0);

static int __init simple_module_init(void)
{
	pr_info("simple_module: start module for loading...\n");
	pr_info("simple_module: wait for loaded module ...\n");
	if (myparam < 0) {
		pr_info("simple_module: loading error\n");
	} else {
		pr_info("simple_module: module is loaded\n");
	}
	pr_info("simple_module: myparam = %d\n", myparam);
	return myparam;
}


static void __exit simple_module_exit(void)
{
	pr_info("simple_module: module removed\n");
}

module_init(simple_module_init);
module_exit(simple_module_exit);
MODULE_AUTHOR("Vladyslav.Kryzhanovskyi");
MODULE_DESCRIPTION("Very simple module for test");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
