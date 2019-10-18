#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>

#define INFO_DBG(x, arg...) \
pr_info("Test_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define INFO_ERROR(x, arg...) \
pr_err("Test_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

static int param = -1;
module_param(param, int, 0600);

static int __init test_module_init(void)
{
	if (param == 0) {
		INFO_DBG("Test module is loaded!\n");
		INFO_DBG("Parameter is %i\n", param);
	} else {
		INFO_ERROR("Test module isn't loaded!\n");
		INFO_ERROR("Parameter is %i\n", param);
	}
	return param;
}

static void __exit test_module_stop(void)
{
	INFO_DBG("Test module is removed!\n");
}

module_init(test_module_init);
module_exit(test_module_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("Test module for linux kernel");
MODULE_VERSION("0.1");
