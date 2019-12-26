#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/of.h>
#include <linux/delay.h>
#include "mpu6050-regs.h"

struct temp_t {
	int t_int;
	int t_frac;
};

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values[3];
	int gyro_values[3];
	struct temp_t temperature;
};

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_data(u8 reg, int *value)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	*value = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, reg));

	return 0;
}

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mpu6050_read_data(REG_ACCEL_XOUT_H, &g_mpu6050_data.accel_values[0]);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);

	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mpu6050_read_data(REG_ACCEL_YOUT_H, &g_mpu6050_data.accel_values[1]);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);

	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mpu6050_read_data(REG_ACCEL_ZOUT_H, &g_mpu6050_data.accel_values[2]);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);

	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mpu6050_read_data(REG_GYRO_XOUT_H, &g_mpu6050_data.gyro_values[0]);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);

	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mpu6050_read_data(REG_GYRO_YOUT_H, &g_mpu6050_data.gyro_values[1]);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);

	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mpu6050_read_data(REG_GYRO_ZOUT_H, &g_mpu6050_data.gyro_values[2]);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);

	return strlen(buf);
}

static ssize_t temp_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;
	int temp = 0;

	mpu6050_read_data(REG_TEMP_OUT_H, &temp);
	g_mpu6050_data.temperature.t_int = (temp * 1000 / 340 + 35000) / 1000;
	g_mpu6050_data.temperature.t_frac = (temp * 1000 / 340 + 35000) % 1000;

	dev_info(&drv_client->dev, "TEMP = %02d.%03d\n",
		g_mpu6050_data.temperature.t_int,
		g_mpu6050_data.temperature.t_frac);

	sprintf(buf, "%02d.%03d\n", g_mpu6050_data.temperature.t_int,
					g_mpu6050_data.temperature.t_frac);
	return strlen(buf);
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temp);

static struct class *attr_class;

static int mpu6050_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	int ret;

	/* Create class */
	attr_class = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		dev_err(&drv_client->dev, "failed to create sysfs class: %d\n",
				ret);
		return ret;
	}
	pr_info("mpu6050: sysfs class created\n");

	/* Create accel_x */
	ret = class_create_file(attr_class, &class_attr_accel_x);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute accel_x: %d\n",
				ret);
		return ret;
	}
	/* Create accel_y */
	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute accel_y: %d\n",
				ret);
		return ret;
	}
	/* Create accel_z */
	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute accel_z: %d\n",
				ret);
		return ret;
	}
	/* Create gyro_x */
	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute gyro_x: %d\n",
				ret);
		return ret;
	}
	/* Create gyro_y */
	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute gyro_y: %d\n",
				ret);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute gyro_z: %d\n",
				ret);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(attr_class, &class_attr_temp);
	if (ret) {
		dev_err(&drv_client->dev,
			"failed to create sysfs class attribute temperature: %d\n",
				ret);
		return ret;
	}

	dev_info(&drv_client->dev, "sysfs class attributes created\n");

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
	// reset
	ret = i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0x80);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed with error: %d\n",
			ret);
		return ret;
	}
	// start
	ret = i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0x00);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed with error: %d\n",
			ret);
		return ret;
	}

	g_mpu6050_data.drv_client = drv_client;

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	g_mpu6050_data.drv_client = 0;

	if (attr_class) {
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_temp);
		dev_info(&drv_client->dev, "sysfs class attributes removed\n");

		class_destroy(attr_class);
		dev_info(&drv_client->dev, "sysfs class destroyed\n");
	}

	dev_info(&drv_client->dev, "i2c driver removed\n");
	return 0;
}

static const struct of_device_id mpu6050_ids[] = {
	{ .compatible = "gl, mpu6050", },
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
		.owner = THIS_MODULE,
		.name = "mpu6050",
		.of_match_table = of_match_ptr(mpu6050_ids),
	},
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

module_i2c_driver(mpu6050_i2c_driver);

MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
