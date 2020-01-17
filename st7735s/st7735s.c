// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  st7735s.c - Display driver
 *
 *  Copyright (C) 2020 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/spi/spi.h>


static int st7735s_probe(struct spi_device *spi)
{
	int ret;

	dev_info(&spi->dev, "%s:%d\n", __func__, __LINE__);

	spi->bits_per_word = 8;
	spi->mode = SPI_MODE_0;
	spi->max_speed_hz = 500000;
	ret = spi_setup(spi);
	if (ret < 0)
		return ret;

	dev_info(&spi->dev, "%s:%d\n", __func__, __LINE__);
	return 0;

}


static int st7735s_remove(struct spi_device *spi)
{
	return 0;
}



static const struct of_device_id of_tbl[] = {
	{.compatible = "st7735s",},
	{},
};

MODULE_DEVICE_TABLE(of, of_tbl);

static struct spi_driver st7735s_driver = {
	.driver = {
		.name		= "st7735s",
		.of_match_table = of_tbl,
		.owner		= THIS_MODULE,
	},

	.probe		= st7735s_probe,
	.remove		= st7735s_remove,
};




static int st7735s_init(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	return spi_register_driver(&st7735s_driver);
}

static void st7735s_exit(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	spi_unregister_driver(&st7735s_driver);
}

module_init(st7735s_init);
module_exit(st7735s_exit);

MODULE_AUTHOR("Andrey Pahomov <pahomov.and@gmail.com>");
MODULE_DESCRIPTION("st7735s SPI display");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
