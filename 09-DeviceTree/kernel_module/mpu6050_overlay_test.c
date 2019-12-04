#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/math64.h>
#include <linux/clocksource.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/time64.h>
#include <linux/tick.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/kdev_t.h>
#include <linux/io.h>
#include <asm-generic/ioctl.h>
#include <linux/kobject.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

#define DBG_INFO(x, arg...) \
pr_info("mpu6050_module_info: function - %s:%i - "x, __func__, __LINE__, ##arg)
#define DBG_ERROR(x, arg...) \
pr_err("mpu6050_module_error: function - %s:%i - "x, __func__, __LINE__, ##arg)

static int mpu6050_test_probe(struct i2c_client *client,
						const struct i2c_device_id *id)
{
	DBG_INFO("mpu6050_test_driver is loaded!\n");
	return 0;
}

static int mpu6050_test_remove(struct i2c_client *client)
{
	DBG_INFO("mpu6050_test_driver is unloaded!\n");
	return 0;
}

static const struct of_device_id mpu6050_dev_dt_ids[] = {
	{ .compatible = "mpu6050_test", },
	{ /* sentinel */ }
};

static struct i2c_driver mpu6050_i2c_drv = {
	.probe = mpu6050_test_probe,
	.remove = mpu6050_test_remove,
	.driver = {
			.name = "mpu6050_test",
			.of_match_table = mpu6050_dev_dt_ids,
			.owner = THIS_MODULE,
	}
};

MODULE_DEVICE_TABLE(of, mpu6050_dev_dt_ids);

module_i2c_driver(mpu6050_i2c_drv);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("Test module for linux kernel");
MODULE_VERSION("0.1");
