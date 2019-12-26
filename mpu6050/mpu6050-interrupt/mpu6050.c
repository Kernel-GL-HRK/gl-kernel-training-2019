#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/device.h>

#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#include "mpu6050-regs.h"

struct mpu6050_data {
	struct i2c_client *drv_client;
	struct workqueue_struct *queue;
	struct work_struct work;
	struct mutex mutex_sysfs;
	struct spinlock spinlock_irq;
	int accel_values[3];
	int gyro_values[3];
	int temperature;
};

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_data(int reg, int *value)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	*value = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, reg));

	return 0;
}

static irqreturn_t interrupt_handler(int irq, void *dev)
{
	spin_lock(&g_mpu6050_data.spinlock_irq);
	queue_work(g_mpu6050_data.queue, &g_mpu6050_data.work);
	spin_unlock(&g_mpu6050_data.spinlock_irq);
	return IRQ_HANDLED;
}

static void work_func(struct work_struct *work)
{
	int temp;

	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	mpu6050_read_data(REG_ACCEL_XOUT_H, &g_mpu6050_data.accel_values[0]);
	mpu6050_read_data(REG_ACCEL_YOUT_H, &g_mpu6050_data.accel_values[1]);
	mpu6050_read_data(REG_ACCEL_ZOUT_H, &g_mpu6050_data.accel_values[2]);
	mpu6050_read_data(REG_GYRO_XOUT_H, &g_mpu6050_data.gyro_values[0]);
	mpu6050_read_data(REG_GYRO_YOUT_H, &g_mpu6050_data.gyro_values[1]);
	mpu6050_read_data(REG_GYRO_ZOUT_H, &g_mpu6050_data.gyro_values[2]);

	mpu6050_read_data(REG_TEMP_OUT_H, &temp);
	g_mpu6050_data.temperature = (temp + 12420);

	pr_info("delay work is active\n");
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
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
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0x80);
	msleep(100);
	i2c_smbus_write_byte_data(drv_client, REG_SMPRT_DIV, 0xFF);
	i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG, 0x90);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0x20);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_2, 0x40);
	i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, 1);

	g_mpu6050_data.drv_client = drv_client;
	mutex_init(&g_mpu6050_data.mutex_sysfs);
	spin_lock_init(&g_mpu6050_data.spinlock_irq);

	g_mpu6050_data.queue = create_singlethread_workqueue("mpu6050_work_queue");
	INIT_WORK(&g_mpu6050_data.work, work_func);

	if (request_irq(drv_client->irq, interrupt_handler,
		IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
		"mpu6050", (void *)drv_client)) {

		dev_info(&drv_client->dev, "interrupt is not registered\n");
		return -EINVAL;

	}
	dev_info(&drv_client->dev, "drv_client->irq = %d\n", drv_client->irq);
	dev_info(&drv_client->dev, "i2c driver probed\n");

	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{

	free_irq(drv_client->irq, (void *)drv_client);
	g_mpu6050_data.drv_client = 0;

	flush_workqueue(g_mpu6050_data.queue);
	destroy_workqueue(g_mpu6050_data.queue);

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
		.name = "gl,mpu6050",
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
	return strlen(buf);
}

static ssize_t temp_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_sysfs);
	sprintf(buf, "%i.%03i\n", g_mpu6050_data.temperature / 340,
					g_mpu6050_data.temperature % 340);
	mutex_unlock(&g_mpu6050_data.mutex_sysfs);
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
		pr_err("mpu6050: failed to create sysfs class attribute accel_x:%d\n",
				ret);
		return ret;
	}
	/* Create accel_y */
	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_y:%d\n",
				ret);
		return ret;
	}
	/* Create accel_z */
	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_z:%d\n",
				ret);
		return ret;
	}
	/* Create gyro_x */
	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_x:%d\n",
				ret);
		return ret;
	}
	/* Create gyro_y */
	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_y:%d\n",
				ret);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_z:%d\n",
				ret);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(attr_class, &class_attr_temp);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute temperature:%d\n",
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
