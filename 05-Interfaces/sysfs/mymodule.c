#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

static int __init init_mod(void)
{
	pr_info("mymodule: module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	pr_info("mymodule: module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");
