// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/completion.h>

#include "mpu6050-regs.h"

static int threshold_param = 5;
module_param(threshold_param, int, 0);

struct mpu6050_data {
	struct i2c_client *drv_client;
	struct mutex mmutex;
	struct workqueue_struct *wqueue;
	struct timespec lt_access; // last time access
	int accel_values[3];
	int gyro_values[3];
	int temperature;
	int temp_factor;
};

static void th_fn_read(struct work_struct *wq);
static DECLARE_DELAYED_WORK(my_work, th_fn_read);
DECLARE_COMPLETION(data_read_done);

static unsigned int onesec;

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_data(void)
{
	int temp;
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
	temp = (s16)((u16)i2c_smbus_read_word_swapped(drv_client,
							REG_TEMP_OUT_H));
	g_mpu6050_data.temperature = (temp + 12420 + 170) / 340;
	g_mpu6050_data.temp_factor = (temp + 12420 + 170) % 340;

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
		g_mpu6050_data.temperature);

	complete_all(&data_read_done);

	return 0;
}

static void check_actual_data(int reg, char *buff)
{
	long diff;
	struct timespec current_time;

	getnstimeofday(&current_time);
	diff = current_time.tv_sec - g_mpu6050_data.lt_access.tv_sec;

	if (diff > threshold_param) {
		if (g_mpu6050_data.wqueue) {
			queue_delayed_work(g_mpu6050_data.wqueue, &my_work,
					onesec);
			wait_for_completion(&data_read_done);
			getnstimeofday(&g_mpu6050_data.lt_access);
		}
	}

	switch (reg) {
	case REG_ACCEL_XOUT_H:
		sprintf(buff, "%d\n", g_mpu6050_data.accel_values[0]);
	break;
	case REG_ACCEL_YOUT_H:
		sprintf(buff, "%d\n", g_mpu6050_data.accel_values[1]);
	break;
	case REG_ACCEL_ZOUT_H:
		sprintf(buff, "%d\n", g_mpu6050_data.accel_values[2]);
	break;
	case REG_GYRO_XOUT_H:
		sprintf(buff, "%d\n", g_mpu6050_data.gyro_values[0]);
	break;
	case REG_GYRO_YOUT_H:
		sprintf(buff, "%d\n", g_mpu6050_data.gyro_values[1]);
	break;
	case REG_GYRO_ZOUT_H:
		sprintf(buff, "%d\n", g_mpu6050_data.gyro_values[2]);
	break;
	case REG_TEMP_OUT_H:
		sprintf(buff, "%d.%03d\n", g_mpu6050_data.temperature, g_mpu6050_data.temp_factor);
	break;
	default:
	break;
	}

}

static void th_fn_read(struct work_struct *wq)
{
	mutex_lock(&g_mpu6050_data.mmutex);
	mpu6050_read_data();
	//complete(&data_read_done);
	mutex_unlock(&g_mpu6050_data.mmutex);
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

	init_completion(&data_read_done);

	onesec = msecs_to_jiffies(1000);
	if (!g_mpu6050_data.wqueue) {
		dev_info(&drv_client->dev, "workqueeue created\n");
		g_mpu6050_data.wqueue =
			create_singlethread_workqueue("thread_mpu6050");
	}

	queue_delayed_work(g_mpu6050_data.wqueue, &my_work, onesec);
	getnstimeofday(&g_mpu6050_data.lt_access);



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
	{ "mpu6050", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "gl,mpu6050",
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_ACCEL_XOUT_H, buf);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_ACCEL_YOUT_H, buf);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_ACCEL_ZOUT_H, buf);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_GYRO_XOUT_H, buf);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_GYRO_YOUT_H, buf);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_GYRO_ZOUT_H, buf);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	return strlen(buf);
}

static ssize_t temperature_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	check_actual_data(REG_TEMP_OUT_H, buf);
	sprintf(buf, "%d.%03d\n", g_mpu6050_data.temperature,
					g_mpu6050_data.temp_factor);
	return strlen(buf);
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
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
	"mpu6050: failed to create sysfs class attribute accel_x: %d\n", ret);
		return ret;
	}
	/* Create accel_y */
	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		pr_err(
	"mpu6050: failed to create sysfs class attribute accel_y: %d\n", ret);
		return ret;
	}
	/* Create accel_z */
	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		pr_err(
	"mpu6050: failed to create sysfs class attribute accel_z: %d\n", ret);
		return ret;
	}
	/* Create gyro_x */
	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		pr_err(
	"mpu6050: failed to create sysfs class attribute gyro_x: %d\n", ret);
		return ret;
	}
	/* Create gyro_y */
	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		pr_err(
	"mpu6050: failed to create sysfs class attribute gyro_y: %d\n", ret);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		pr_err(
	"mpu6050: failed to create sysfs class attribute gyro_z: %d\n", ret);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(attr_class, &class_attr_temperature);
	if (ret) {
		pr_err(
	"mpu6050: failed to create sysfs class attribute temperature: %d\n",
ret);
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

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
