// SPDX-License-Identifier: GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "mpu6050-regs.h"

#define RD_WORD_SW(DRV, REG)			\
	((s16)((u16)i2c_smbus_read_word_swapped(DRV, REG)))

struct mpu6050_axic {
	int x;
	int y;
	int z;
};

struct numb_descr {
	int integer;
	int fractional;
};

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values[3];
	int gyro_values[3];
	struct numb_descr temperature;
};

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_accel(struct mpu6050_axic *accel)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;
	int status;
	s16 values[3];

	if (drv_client == 0)
		return -ENODEV;
	status = i2c_smbus_read_i2c_block_data(drv_client,
					 REG_ACCEL_XOUT_H, sizeof(values),
					 (u8 *)&values);
	if (status != sizeof(values))
		return -ERANGE;

	accel->x = (s16)swab16(values[0]);
	accel->y = (s16)swab16(values[1]);
	accel->z = (s16)swab16(values[2]);

	return status;
}

static int mpu6050_read_gyro(struct mpu6050_axic *gyro)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;
	int status;
	s16 values[3];

	if (drv_client == 0)
		return -ENODEV;

	status = i2c_smbus_read_i2c_block_data(drv_client,
					 REG_GYRO_XOUT_H, sizeof(values),
					 (u8 *)&values);
	if (status != sizeof(values))
		return -ERANGE;

	gyro->x = (s16)swab16(values[0]);
	gyro->y = (s16)swab16(values[1]);
	gyro->z = (s16)((u16)swab16(values[2]));

	return status;
}

static int mpu6050_read_value(uint8_t reg)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;
	int value;

	if (drv_client == 0)
		return -ENODEV;
	value = RD_WORD_SW(drv_client, reg);
	dev_info(&drv_client->dev,
		"mpu6050_rd: REG:0x%X Val:%d\n", reg, value);
	return value;
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

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	g_mpu6050_data.drv_client = 0;

	dev_info(&drv_client->dev, "i2c driver removed\n");
	return 0;
}

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "gl_mpu6050",
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	int value;

	value = mpu6050_read_value(REG_ACCEL_XOUT_H);

	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	int value;

	value = mpu6050_read_value(REG_ACCEL_YOUT_H);

	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	int value;

	value = mpu6050_read_value(REG_ACCEL_ZOUT_H);

	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t accel_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	struct mpu6050_axic accel;
	int status;

	status = mpu6050_read_accel(&accel);

	if (status < 0)
		return status;

	sprintf(buf, "x:%d\ny:%d\nz:%d\n", accel.x, accel.y, accel.z);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	int value;

	value = mpu6050_read_value(REG_GYRO_XOUT_H);

	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	int value;

	value = mpu6050_read_value(REG_GYRO_YOUT_H);

	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	int value;

	value = mpu6050_read_value(REG_GYRO_ZOUT_H);

	sprintf(buf, "%d\n", value);
	return strlen(buf);
}

static ssize_t gyro_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	struct mpu6050_axic gyro;
	int status;

	status = mpu6050_read_gyro(&gyro);

	if (status < 0)
		return status;

	sprintf(buf, "x:%d\ny:%d\nz:%d\n", gyro.x, gyro.y, gyro.z);
	return strlen(buf);
}

static ssize_t temperature_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	int value;
	struct numb_descr temp;

	value = mpu6050_read_value(REG_TEMP_OUT_H);

	temp.integer =
		(value * 1000 / 340 + 35000) / 1000;
	temp.fractional =
		(value * 1000 / 340 + 35000) % 1000;

	sprintf(buf, "%02d.%03d\n",
		temp.integer,
		temp.fractional);
	return strlen(buf);
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(accel);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(gyro);
CLASS_ATTR_RO(temperature);

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
		pr_err(
			"mpu6050: failed to create sysfs class attribute accel_x: %d\n",
			ret);
		class_destroy(attr_class);
		return ret;
	}
	/* Create accel_y */
	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute accel_y: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		return ret;
	}
	/* Create accel_z */
	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute accel_z: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		return ret;
	}
	/* Create accel */
	ret = class_create_file(attr_class, &class_attr_accel);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute accel: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		return ret;
	}
	/* Create gyro_x */
	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute gyro_x: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_accel);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_destroy(attr_class);
		return ret;
	}
	/* Create gyro_y */
	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute gyro_y: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_destroy(attr_class);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute gyro_z: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_destroy(attr_class);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute gyro_z: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_destroy(attr_class);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(attr_class, &class_attr_temperature);
	if (ret) {
		pr_err(
			"mpu6050: failed to create sysfs class attribute temperature: %d\n",
			ret);
		class_remove_file(attr_class, &class_attr_gyro);
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_accel);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_destroy(attr_class);
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
		class_remove_file(attr_class, &class_attr_accel);
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro);
		class_remove_file(attr_class, &class_attr_temperature);
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

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com> ");
MODULE_AUTHOR("Dmytro.Topikha <dmytro.topikha@gmail.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
