#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>

#include "mpu6050-regs.h"

#define UPDATE_INTERVAL_MS  100

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values[3];
	int gyro_values[3];
	int tempInt;
	int tempFract;
	unsigned long lastUpdateJiffies;
	unsigned long updateIntervalJiffies;
	unsigned int updateIntervalMs;
};

static struct mpu6050_data g_mpu6050_data;
static struct task_struct *updateThread;
static DECLARE_COMPLETION(dataIsUpdated);
static DEFINE_MUTEX(readingIsBlocked);

static int mpu6050_read_data(void)
{
	int tmp;
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	/* accel */
	g_mpu6050_data.accel_values[0] =
	(s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
	g_mpu6050_data.accel_values[1] =
	(s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
	g_mpu6050_data.accel_values[2] =
	(s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));

	/* gyro */
	g_mpu6050_data.gyro_values[0] =
	(s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
	g_mpu6050_data.gyro_values[1] =
	(s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
	g_mpu6050_data.gyro_values[2] =
	(s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));
	/* Temperature in degrees C =
	 * (TEMP_OUT Register Value  as a signed quantity)/340 + 36.53
	 */
	tmp = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, 
		REG_TEMP_OUT_H));
	tmp += 12420;
	g_mpu6050_data.tempInt = tmp / 340;
	g_mpu6050_data.tempFract = (tmp % 340) * 100 / 34;

	dev_info(&drv_client->dev, "sensor data read:\n");
	dev_info(&drv_client->dev, "ACCEL[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.accel_values[0],
		g_mpu6050_data.accel_values[1],
		g_mpu6050_data.accel_values[2]);
	dev_info(&drv_client->dev, "GYRO[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.gyro_values[0],
		g_mpu6050_data.gyro_values[1],
		g_mpu6050_data.gyro_values[2]);
	dev_info(&drv_client->dev, "TEMP = %d\n",
		g_mpu6050_data.tempInt);

	return 0;
}

static void checkDataUpdate(void)
{
	if (jiffies - g_mpu6050_data.lastUpdateJiffies >
		g_mpu6050_data.updateIntervalJiffies) {
		wake_up_process(updateThread);
		wait_for_completion(&dataIsUpdated);
	}
}

static int threadRoutine(void *data)
{
	while (!kthread_should_stop()) {
		mpu6050_read_data();
		g_mpu6050_data.lastUpdateJiffies = jiffies;
		complete(&dataIsUpdated);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}

	return 0;
}

static int mpu6050_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	int ret;

	dev_info(&drv_client->dev,
		"i2c client address is 0x%X\n", drv_client->addr);

	/* Read who_am_i register */
	ret = i2c_smbus_read_byte_data(drv_client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_read_byte_data() failed with error: %d\n",
			ret);
		return ret;
	}
	if (ret != MPU6050_WHO_AM_I) {
		dev_err(&drv_client->dev,
			"wrong i2c device found: expected 0x%X, found 0x%X\n",
			MPU6050_WHO_AM_I, ret);
		return -1;
	}
	dev_info(&drv_client->dev,
		"i2c mpu6050 device found, WHO_AM_I register value = 0x%X\n",
		ret);

	/* Setup the device */
	/* No error handling here! */
	i2c_smbus_write_byte_data(drv_client, REG_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_GYRO_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_FIFO_EN, 0);
	i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, 0);
	i2c_smbus_write_byte_data(drv_client, REG_USER_CTRL, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_2, 0);

	g_mpu6050_data.drv_client = drv_client;

	if(of_find_property(drv_client->dev.of_node, "update_interval", NULL)) {
		of_property_read_u32(drv_client->dev.of_node, "update_interval",
			&g_mpu6050_data.updateIntervalMs);
	} else {
		g_mpu6050_data.updateIntervalMs = UPDATE_INTERVAL_MS;
	}

	g_mpu6050_data.updateIntervalJiffies =
		HZ * g_mpu6050_data.updateIntervalMs / 1000;

	updateThread = kthread_run(threadRoutine, &g_mpu6050_data,
								"thread");

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	g_mpu6050_data.drv_client = 0;

	kthread_stop(updateThread);

	dev_info(&drv_client->dev, "i2c driver removed\n");
	return 0;
}

static const struct of_device_id mpu6050_ids[] = {
	{ .compatible = "mpu6050 i2c driver", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mpu6050_ids);

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "gl_mpu6050",
		.of_match_table = of_match_ptr(mpu6050_ids),
		.owner = THIS_MODULE,
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

static ssize_t temp_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mutex_lock(&readingIsBlocked);

	checkDataUpdate();
	sprintf(buf, "%i.%03i\n", g_mpu6050_data.tempInt,
		g_mpu6050_data.tempFract);

	mutex_unlock(&readingIsBlocked);

	return strlen(buf);
}

struct class_attribute class_attr_accel_x = {
	.attr = { .name = "accel_x", .mode = 0666 },
	.show	= accel_x_show,
};

struct class_attribute class_attr_accel_y = {
	.attr = { .name = "accel_y", .mode = 0666 },
	.show	= accel_y_show,
};

struct class_attribute class_attr_accel_z = {
	.attr = { .name = "accel_z", .mode = 0666 },
	.show	= accel_z_show,
};

struct class_attribute class_attr_gyro_x = {
	.attr = { .name = "gyro_x", .mode = 0666 },
	.show	= gyro_x_show,
};

struct class_attribute class_attr_gyro_y = {
	.attr = { .name = "gyro_y", .mode = 0666 },
	.show	= gyro_y_show,
};

struct class_attribute class_attr_gyro_z = {
	.attr = { .name = "gyro_z", .mode = 0666 },
	.show	= gyro_z_show,
};

struct class_attribute class_attr_temp = {
	.attr = { .name = "temp", .mode = 0666 },
	.show	= temp_show,
};

static struct class *attr_class;

static int mpu6050_init(void)
{
	int ret;

	/* Create i2c driver */
	ret = i2c_add_driver(&mpu6050_i2c_driver);
	if (ret) {
		pr_err("mpu6050: failed to add new i2c driver: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: i2c driver created\n");

	/* Create class */
	attr_class = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		pr_err("mpu6050: failed to create sysfs class: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: sysfs class created\n");

	/* Create accel_x */
	ret = class_create_file(attr_class, &class_attr_accel_x);
	if (ret) {
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute accel_x:%d\n", ret);
		return ret;
	}
	/* Create accel_y */
	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute accel_y: %d\n", ret);
		return ret;
	}
	/* Create accel_z */
	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute accel_z: %d\n", ret);
		return ret;
	}
	/* Create gyro_x */
	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute gyro_x: %d\n", ret);
		return ret;
	}
	/* Create gyro_y */
	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute gyro_y: %d\n", ret);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute gyro_z: %d\n", ret);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(attr_class, &class_attr_temp);
	if (ret) {
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		pr_err("mpu6050: failed to create sysfs class attribute temperature: %d\n", ret);
		return ret;
	}

	pr_info("mpu6050: sysfs class attributes created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
	if (attr_class) {
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_temp);
		pr_info("mpu6050: sysfs class attributes removed\n");

		class_destroy(attr_class);
		pr_info("mpu6050: sysfs class destroyed\n");
	}

	i2c_del_driver(&mpu6050_i2c_driver);
	pr_info("mpu6050: i2c driver deleted\n");

	pr_info("mpu6050: module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
