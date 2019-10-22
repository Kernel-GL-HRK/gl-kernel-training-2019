#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Dolinin");
MODULE_DESCRIPTION("Simple Linux module");
MODULE_VERSION("0.1");

static int myparam;
module_param(myparam, int, 0000);

static int __init init_mod(void)
{
	pr_info("Init simple Linux module\n");
	pr_info("Input parameter = %d\n", myparam);
	if (myparam == 666) {
		pr_info("Something went wrong ...");
		return -1;
	}
	return 0;
}

static void __exit exit_mod(void)
{
	pr_info("Remove simple Linux module\n");
}

module_init(init_mod);
module_exit(exit_mod);
