#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/moduleparam.h>
#include <linux/completion.h>
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
	struct mutex mutex_mpu;
	struct completion read_complete;
	struct task_struct *reading_thread;
	unsigned int delay_ms;
	struct timer_list delay_timer;
	bool flag_read;
};

static struct mpu6050_data g_mpu6050_data;

static void delay_timer_callback(struct timer_list *t)
{
	dev_info(&g_mpu6050_data.drv_client->dev,
		"delay timer callback, data can be updated\n");
	g_mpu6050_data.flag_read = false;
}

static int mpu6050_read_data(void)
{
	int temp = 0;
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	dev_info(&drv_client->dev, "sensor data read:\n");

	/* accel */
	g_mpu6050_data.accel_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
	g_mpu6050_data.accel_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
	g_mpu6050_data.accel_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));
	/* gyro */
	g_mpu6050_data.gyro_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
	g_mpu6050_data.gyro_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
	g_mpu6050_data.gyro_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));
	/* temp */
	temp = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H));
	g_mpu6050_data.temperature.t_int = (temp * 1000 / 340 + 35000) / 1000;
	g_mpu6050_data.temperature.t_frac = (temp * 1000 / 340 + 35000) % 1000;

	dev_info(&drv_client->dev, "ACCEL[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.accel_values[0],
		g_mpu6050_data.accel_values[1],
		g_mpu6050_data.accel_values[2]);
	dev_info(&drv_client->dev, "GYRO[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.gyro_values[0],
		g_mpu6050_data.gyro_values[1],
		g_mpu6050_data.gyro_values[2]);
	dev_info(&drv_client->dev, "TEMP = %02d.%03d\n",
		g_mpu6050_data.temperature.t_int,
		g_mpu6050_data.temperature.t_frac);

	return 0;
}

static int mpu6050_read_thread(void *data)
{
	struct mpu6050_data *pdata = data;

	while (!kthread_should_stop()) {
		dev_info(&pdata->drv_client->dev, "mpu6050 read thread\n");

		mutex_lock(&pdata->mutex_mpu);
		mpu6050_read_data();
		mutex_unlock(&pdata->mutex_mpu);

		complete(&pdata->read_complete);
		mod_timer(&pdata->delay_timer, get_jiffies_64() +
				msecs_to_jiffies(pdata->delay_ms));
		pdata->flag_read = true;

		kthread_park(pdata->reading_thread);
		if (kthread_should_park())
			kthread_parkme();
	}
	return 0;
}

static ssize_t accel_x_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}

	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}

	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}

	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}

	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}

	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t temp_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	if (!g_mpu6050_data.flag_read) {
		kthread_unpark(g_mpu6050_data.reading_thread);
		wake_up_process(g_mpu6050_data.reading_thread);
		wait_for_completion(&g_mpu6050_data.read_complete);
	}

	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%02d.%03d\n", g_mpu6050_data.temperature.t_int,
					g_mpu6050_data.temperature.t_frac);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t delay_ms_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.delay_ms);

	return strlen(buf);
}

static ssize_t delay_ms_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	ssize_t result = 0;
	unsigned int delay = 0;

	result = sscanf(buf, "%d", &delay);
	if (result != 1)
		return -EINVAL;

	g_mpu6050_data.delay_ms = delay;
	dev_info(&g_mpu6050_data.drv_client->dev,
		"set new delay value: %d mS\n", delay);

	return count;
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temp);
CLASS_ATTR_RW(delay_ms);

static struct attribute *mpu_class_attrs[] = {
	&class_attr_accel_x.attr,
	&class_attr_accel_y.attr,
	&class_attr_accel_z.attr,
	&class_attr_gyro_x.attr,
	&class_attr_gyro_y.attr,
	&class_attr_gyro_z.attr,
	&class_attr_temp.attr,
	&class_attr_delay_ms.attr,
	NULL,
};

ATTRIBUTE_GROUPS(mpu_class);

/* Device model classes */
struct class mpu_rc_class = {
	.name = "mpu6050",
	.owner = THIS_MODULE,
	.class_groups = mpu_class_groups,
};

static int mpu6050_probe(struct i2c_client *drv_client,
			const struct i2c_device_id *id)
{
	int ret;

	g_mpu6050_data.drv_client = drv_client;

	mutex_init(&g_mpu6050_data.mutex_mpu);

	init_completion(&g_mpu6050_data.read_complete);

	g_mpu6050_data.reading_thread = kthread_create(mpu6050_read_thread,
				&g_mpu6050_data, "reading_mpu6050");

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
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_PWR_MGMT_1, 0x80))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}
	msleep(100);
	// start
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_PWR_MGMT_1, 0x00))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}

	if (of_find_property(drv_client->dev.of_node, "delay_ms", NULL)) {
		of_property_read_u32(drv_client->dev.of_node, "delay_ms",
			&g_mpu6050_data.delay_ms);
		dev_info(&drv_client->dev, "dtsi->delay_ms = %d\n",
			g_mpu6050_data.delay_ms);
	} else {
		g_mpu6050_data.delay_ms = 10000;
	}

	timer_setup(&g_mpu6050_data.delay_timer, delay_timer_callback, 0);

	ret = class_register(&mpu_rc_class);
	if (ret < 0) {
		dev_err(&drv_client->dev, "failed to create sysfs class: %d\n",
			ret);
		return ret;
	}
	dev_info(&drv_client->dev, "sysfs class created\n");

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	g_mpu6050_data.drv_client = 0;

	class_unregister(&mpu_rc_class);
	dev_info(&drv_client->dev, "sysfs class destroyed\n");

	del_timer(&g_mpu6050_data.delay_timer);
	kthread_stop(g_mpu6050_data.reading_thread);
	mutex_destroy(&g_mpu6050_data.mutex_mpu);

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

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com>");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
