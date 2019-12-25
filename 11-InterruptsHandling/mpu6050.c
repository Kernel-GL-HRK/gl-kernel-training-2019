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
#include "mpu6050-regs.h"

struct temp_t {
	int temp;
	int t_int;
	int t_frac;
};

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values[3];
	int gyro_values[3];
	struct temp_t temperature;
	struct mutex mutex_mpu;
	int avr_acc_x[10], avr_acc_y[10], avr_acc_z[10];
	int avr_gyro_x[10], avr_gyro_y[10], avr_gyro_z[10];
	int avr_temp[10];
};

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_data(void)
{
	int ret = 0, i = 0;
	long val[7] = {0};
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	// Disable INT
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_INT_ENABLE, 0x00))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}

	ret = i2c_smbus_read_byte_data(drv_client, REG_INT_STATUS);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_read_byte_data() failed\n");
		return -EIO;
	}
	dev_info(&drv_client->dev, "REG_INT_STATUS: 0x%x\n", ret);

	if (ret & 0x40) {
		for (i = 0; i < 10; i++) {
			if (i == 9) {
				/* accel */
				g_mpu6050_data.avr_acc_x[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
				g_mpu6050_data.avr_acc_y[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
				g_mpu6050_data.avr_acc_z[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));
				/* gyro */
				g_mpu6050_data.avr_gyro_x[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
				g_mpu6050_data.avr_gyro_y[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
				g_mpu6050_data.avr_gyro_z[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));
				/* temp */
				g_mpu6050_data.avr_temp[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H));
			} else {
				g_mpu6050_data.avr_acc_x[i] = g_mpu6050_data.avr_acc_x[i+1];
				g_mpu6050_data.avr_acc_y[i] = g_mpu6050_data.avr_acc_y[i+1];
				g_mpu6050_data.avr_acc_z[i] = g_mpu6050_data.avr_acc_z[i+1];
				g_mpu6050_data.avr_gyro_x[i] = g_mpu6050_data.avr_gyro_x[i+1];
				g_mpu6050_data.avr_gyro_y[i] = g_mpu6050_data.avr_gyro_y[i+1];
				g_mpu6050_data.avr_gyro_z[i] = g_mpu6050_data.avr_gyro_z[i+1];
				g_mpu6050_data.avr_temp[i] = g_mpu6050_data.avr_temp[i+1];
			}
			msleep(1);
		}
		for (i = 0; i < 10; i++) {
			val[0] += g_mpu6050_data.avr_acc_x[i];
			val[1] += g_mpu6050_data.avr_acc_y[i];
			val[2] += g_mpu6050_data.avr_acc_z[i];
			val[3] += g_mpu6050_data.avr_gyro_x[i];
			val[4] += g_mpu6050_data.avr_gyro_y[i];
			val[5] += g_mpu6050_data.avr_gyro_z[i];
			val[6] += g_mpu6050_data.avr_temp[i];
		}
		g_mpu6050_data.accel_values[0] = val[0] / 10;
		g_mpu6050_data.accel_values[1] = val[1] / 10;
		g_mpu6050_data.accel_values[2] = val[2] / 10;
		g_mpu6050_data.gyro_values[0] = val[3] / 10;
		g_mpu6050_data.gyro_values[1] = val[4] / 10;
		g_mpu6050_data.gyro_values[2] = val[5] / 10;
		g_mpu6050_data.temperature.temp = val[6] / 10;

		g_mpu6050_data.temperature.t_int = (g_mpu6050_data.temperature.temp * 1000 / 340 + 35000) / 1000;
		g_mpu6050_data.temperature.t_frac = (g_mpu6050_data.temperature.temp * 1000 / 340 + 35000) % 1000;

		dev_info(&drv_client->dev, "sensor data read:\n");
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
	}

	// Enable INT
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_INT_ENABLE, 0x40))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}

	return 0;
}

static irqreturn_t interrupt_handler(int irq, void *dev_id)
{
	dev_info(&((struct mpu6050_data *)dev_id)->drv_client->dev,
		"**************** Interrupt generated #%d ****************",
		((struct mpu6050_data *)dev_id)->drv_client->irq);

	return IRQ_WAKE_THREAD;
}

static irqreturn_t interrupt_thread(int irq, void *dev_id)
{
	mutex_lock(&((struct mpu6050_data *)dev_id)->mutex_mpu);
	mpu6050_read_data();
	mutex_unlock(&((struct mpu6050_data *)dev_id)->mutex_mpu);

	return IRQ_HANDLED;
}

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

static ssize_t temp_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mutex_lock(&g_mpu6050_data.mutex_mpu);
	sprintf(buf, "%02d.%03d\n", g_mpu6050_data.temperature.t_int,
					g_mpu6050_data.temperature.t_frac);
	mutex_unlock(&g_mpu6050_data.mutex_mpu);

	return strlen(buf);
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temp);

static struct attribute *mpu_class_attrs[] = {
	&class_attr_accel_x.attr,
	&class_attr_accel_y.attr,
	&class_attr_accel_z.attr,
	&class_attr_gyro_x.attr,
	&class_attr_gyro_y.attr,
	&class_attr_gyro_z.attr,
	&class_attr_temp.attr,
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
	int ret = 0, i = 0;

	mutex_init(&g_mpu6050_data.mutex_mpu);

	dev_info(&drv_client->dev,
		"i2c client address is 0x%X\n", drv_client->addr);

	/* Read who_am_i register */
	ret = i2c_smbus_read_byte_data(drv_client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_read_byte_data() failed\n");
		return -EIO;
	}
	if (ret != MPU6050_WHO_AM_I) {
		dev_err(&drv_client->dev,
			"wrong i2c device found: expected 0x%X, found 0x%X\n",
			MPU6050_WHO_AM_I, ret);
		return -ENODEV;
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
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_CONFIG, 0x03))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_SMPRT_DIV, 0x04))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_MOT_THR, 0x15))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_MOT_DUR, 0x02))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_INT_PIN_CFG, 0xB2))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}
	if (IS_ERR_VALUE(i2c_smbus_write_byte_data(drv_client,
		REG_INT_ENABLE, 0x40))) {
		dev_err(&drv_client->dev,
			"i2c_smbus_write_byte_data() failed\n");
		return -EIO;
	}

	ret = request_threaded_irq(drv_client->irq, interrupt_handler,
		interrupt_thread, IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
		"mpu6050", (void *)&g_mpu6050_data);
	if (ret) {
		dev_err(&drv_client->dev,
			"error irq <request_threaded_irq>, unable to request IRQ#%d: %d\n",
			drv_client->irq, ret);
		return ret;
	}

	dev_info(&drv_client->dev, "IRQ #%d\n", drv_client->irq);

	ret = class_register(&mpu_rc_class);
	if (ret < 0) {
		dev_err(&drv_client->dev, "failed to create sysfs class: %d\n",
			ret);
		return ret;
	}
	dev_info(&drv_client->dev, "sysfs class created\n");

	// Read data for update
	for (i = 0; i < 10; i++) {
		/* accel */
		g_mpu6050_data.avr_acc_x[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
		g_mpu6050_data.avr_acc_y[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
		g_mpu6050_data.avr_acc_z[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));
		/* gyro */
		g_mpu6050_data.avr_gyro_x[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
		g_mpu6050_data.avr_gyro_y[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
		g_mpu6050_data.avr_gyro_z[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));
		/* temp */
		g_mpu6050_data.avr_temp[i] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H));
	}

	g_mpu6050_data.drv_client = drv_client;

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	g_mpu6050_data.drv_client = 0;

	class_unregister(&mpu_rc_class);
	dev_info(&drv_client->dev, "sysfs class destroyed\n");

	free_irq(drv_client->irq, (void *)&g_mpu6050_data);

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

MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro interrupt");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
