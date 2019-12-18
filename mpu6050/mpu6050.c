// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  module_fake_08.c - Test module
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/list.h>

#include "mpu6050-regs.h"

#define LEN_DATA ((REG_GYRO_ZOUT_L - REG_ACCEL_XOUT_H)+1)


static struct task_struct *i2c_read_thread;
static u32 dalay_ms = 2000;
static bool isDebug;
static struct timespec old_time;

static struct semaphore s_lock;

static LIST_HEAD(mpu6050_data_list);

module_param(dalay_ms, uint, 0);


struct temp_float {
	int integer;
	int frac;
};

union data_map {
	struct {
		s16 accel_x;
		s16 accel_y;
		s16 accel_z;
		s16 temp;
		s16 gyro_x;
		s16 gyro_y;
		s16 gyro_z;
	};

	u8 data[LEN_DATA];

};

struct mpu6050_data {

	struct i2c_client *drv_client;
	struct kobject kobj;

	int accel_values[3];
	int gyro_values[3];
	struct temp_float temperature;

	union data_map g_data_map;

	struct task_struct *i2c_read_thread;

	// struct mutex m_lock;
	struct completion thred_wait;

	struct list_head list;
};

static inline struct temp_float temp_convert(s16 temp)
{
	struct temp_float ret;

	ret.integer = (temp + 12420 + 170);
	ret.frac = ret.integer % 340;
	ret.integer /= 340;

	return ret;
}


static int mpu6050_read_data(struct mpu6050_data *g_mpu6050_data)
{
	s32 len;

	struct i2c_client *drv_client = g_mpu6050_data->drv_client;

	if (drv_client == 0)
		return -ENODEV;

	up(&s_lock);

	len = i2c_smbus_read_i2c_block_data(drv_client,
		     REG_ACCEL_XOUT_H, LEN_DATA,
		     g_mpu6050_data->g_data_map.data);

	g_mpu6050_data->accel_values[0] =
		(s16)swab16(g_mpu6050_data->g_data_map.accel_x);

	g_mpu6050_data->accel_values[1] =
		(s16)swab16(g_mpu6050_data->g_data_map.accel_y);

	g_mpu6050_data->accel_values[2] =
		(s16)swab16(g_mpu6050_data->g_data_map.accel_z);

	g_mpu6050_data->temperature =
		temp_convert(swab16(g_mpu6050_data->g_data_map.temp));

	g_mpu6050_data->gyro_values[0] =
		(s16)swab16(g_mpu6050_data->g_data_map.gyro_x);

	g_mpu6050_data->gyro_values[1] =
		(s16)swab16(g_mpu6050_data->g_data_map.gyro_y);

	g_mpu6050_data->gyro_values[2] =
		(s16)swab16(g_mpu6050_data->g_data_map.gyro_z);

	down(&s_lock);

	return 0;
}

static int mpu6050_thread(void *p)
{
	struct mpu6050_data *g_mpu6050_data = p;

	if (g_mpu6050_data)
		mpu6050_read_data(g_mpu6050_data);

	complete(&g_mpu6050_data->thred_wait);

	return 0;
}


// #define SEC_IN_NS 1000000000
// #define SEC_IN_MKS 1000000
#define SEC_IN_MS 1000

static ssize_t all_show(struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf)
{
	struct timespec  cur_time;
	u64 cur_ns, old_ns;
	struct mpu6050_data *g_mpu6050_data;

	getnstimeofday(&cur_time);

	cur_ns = cur_time.tv_sec * 1000 + cur_time.tv_nsec/1000000;
	old_ns = old_time.tv_sec * 1000 + old_time.tv_nsec/1000000;

	if ((cur_ns - old_ns) > dalay_ms) {
		pr_info("read i2c. dalay_ms:%d\tdt:%lld\n",
			dalay_ms, cur_ns - old_ns);

		old_time = cur_time;

		list_for_each_entry(g_mpu6050_data, &mpu6050_data_list, list) {
			if (strcmp(g_mpu6050_data->drv_client->dev.kobj.name,
					kobj->name) == 0) {

				i2c_read_thread = kthread_run(mpu6050_thread,
					g_mpu6050_data,
					"mpu6050_i2c_read_thread");
				wait_for_completion(
					&g_mpu6050_data->thred_wait);
				break;
			}
		}
	} else {
		pr_info("read array. dalay_ms: %d\tdt:%lld\n",
			dalay_ms, cur_ns - old_ns);
	}


	list_for_each_entry(g_mpu6050_data, &mpu6050_data_list, list) {

		if (strcmp(g_mpu6050_data->drv_client->dev.kobj.name,
			kobj->name) == 0) {

			if (strcmp(attr->attr.name, "temp") == 0) {

				sprintf(buf,
				"%d.%d\n",
				g_mpu6050_data->temperature.integer,
				g_mpu6050_data->temperature.frac
				);

			} else if (strcmp(attr->attr.name, "gyro_xyz") == 0) {

				sprintf(buf,
				"%d\t"
				"%d\t"
				"%d\n",
				g_mpu6050_data->gyro_values[0],
				g_mpu6050_data->gyro_values[1],
				g_mpu6050_data->gyro_values[2]
				);

			} else if (strcmp(attr->attr.name, "accel_xyz") == 0) {

				sprintf(buf,
				"%d\t"
				"%d\t"
				"%d\n",
				g_mpu6050_data->accel_values[0],
				g_mpu6050_data->accel_values[1],
				g_mpu6050_data->accel_values[2]
				);
			}

			break;
		}
	}
	return strlen(buf);
}


static struct kobj_attribute temp_attribute =
	__ATTR(temp, 0444, all_show, NULL);

static struct kobj_attribute gyro_xyz_attribute =
	__ATTR(gyro_xyz, 0444, all_show, NULL);

static struct kobj_attribute accel_xyz_attribute =
	__ATTR(accel_xyz, 0444, all_show, NULL);

static struct attribute *attrs[] = {
	&temp_attribute.attr,
	&gyro_xyz_attribute.attr,
	&accel_xyz_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};


static int mpu6050_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	int ret;
	struct device_node *np =  drv_client->dev.of_node;

	struct mpu6050_data *g_mpu6050_data;

	g_mpu6050_data = kzalloc(sizeof(struct mpu6050_data), GFP_KERNEL);
	if (!g_mpu6050_data)
		return -ENOMEM;


	g_mpu6050_data->kobj =  drv_client->dev.kobj;
	g_mpu6050_data->drv_client = drv_client;

	init_completion(&g_mpu6050_data->thred_wait);

	dev_info(&drv_client->dev, "probeing\n");

	dev_info(&drv_client->dev,
		"i2c client address is 0x%X\n", drv_client->addr);

	isDebug = of_get_property(np, "debug", NULL) ? true : false;

	if (of_get_property(np, "dalay_ms", NULL)) {

		of_property_read_u32(np, "dalay_ms", &dalay_ms);

		if (dalay_ms > 5000)
			dalay_ms = 5000;

	}

	dev_info(&drv_client->dev, "dalay_ms = %u\n", dalay_ms);

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


	list_add_tail(&g_mpu6050_data->list, &mpu6050_data_list);

	ret = sysfs_create_group(&g_mpu6050_data->kobj, &attr_group);
	if (ret)
		dev_err(&drv_client->dev, "sysfs_create_group\n");

	getnstimeofday(&old_time);

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	struct mpu6050_data *g_mpu6050_data;

	list_for_each_entry(g_mpu6050_data, &mpu6050_data_list, list) {
		if (strcmp(g_mpu6050_data->drv_client->dev.kobj.name,
			drv_client->dev.kobj.name) == 0) {

			kfree(g_mpu6050_data);
			break;
		}

	}

	dev_info(&drv_client->dev, "i2c driver removed\n");
	return 0;
}

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

#ifdef CONFIG_OF
static const struct of_device_id of_tbl[] = {
	{.compatible = "mpu6050",},
	{},
};

MODULE_DEVICE_TABLE(of, of_tbl);
#endif

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "gl_mpu6050",
#ifdef CONFIG_OF
		.of_match_table = of_tbl,
#endif
		.owner = THIS_MODULE,
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static int mpu6050_init(void)
{
	int ret;

	pr_info("initing\n");
	/* Create i2c driver */
	ret = i2c_add_driver(&mpu6050_i2c_driver);
	if (ret) {
		pr_err("mpu6050: failed to add new i2c driver: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: i2c driver created\n");

	sema_init(&s_lock, 0);

	pr_info("mpu6050: sysfs class attributes created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
	struct mpu6050_data *g_mpu6050_data;

	list_for_each_entry(g_mpu6050_data, &mpu6050_data_list, list) {
			sysfs_remove_group(
				&g_mpu6050_data->drv_client->dev.kobj,
				&attr_group);
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
