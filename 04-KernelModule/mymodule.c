#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>

static int myparam;

module_param(myparam, int, 0);

static int __init mymodule_init(void)
{
	pr_info("mymodule: module is loading...\n");
	if (myparam >= 0) {
		pr_info("mymodule: module is loaded\n");
	} else {
		pr_info("mymodule: loading error\n");
	}
	pr_info("mymodule: myparam = %d\n", myparam);
	return myparam;
}

static void __exit mymodule_exit(void)
{
	pr_info("mymodule: module removed\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Anton Degtyarov");
MODULE_DESCRIPTION("Simple module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
