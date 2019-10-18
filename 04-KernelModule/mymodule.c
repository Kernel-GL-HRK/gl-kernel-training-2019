#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int par;
module_param(par, int, 0);

static int __init init_mod(void)
{
	pr_info("Init module\n");
	if (par)
		pr_info("Input parameter = %d\n", par);
	return par;
}

static void __exit exit_mod(void)
{
	pr_info("Remove module\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuriy Podgorniy");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");
