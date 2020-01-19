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
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/of.h>

#define ST7735S_MADCTL_RGB 0x00
#define ST7735S_MADCTL_BGR 0x08
#define ST7735S_MADCTL_MY  0x80
#define ST7735S_MADCTL_MX  0x40

#define ST7735S_IS_160X128 1
#define ST7735S_WIDTH  128
#define ST7735S_HEIGHT 160
#define ST7735S_XSTART 0
#define ST7735S_YSTART 0
#define ST7735S_ROTATION (ST7735S_MADCTL_MX | ST7735S_MADCTL_MY)

// Pins
#define ST7735S_PIN_CS 67
#define ST7735S_PIN_RESET 2
#define ST7735S_PIN_DC 71

// Commands
#define ST7735S_SWRESET 0x01 // SoftWare Reset
#define ST7735S_SLPOUT 0x11 // Sleep Out
#define ST7735S_FRMCTR1 0xB1 // Norman Mode (Full colors)
#define ST7735S_FRMCTR2 0xB2 // In idle Mode (8 colors)
#define ST7735S_FRMCTR3 0xB3 // In partial Mode + Full Colors
#define ST7735S_INVCTR 0xB4 // Display inversion control
#define ST7735S_PWCTR1 0xC0 // Power control setting
#define ST7735S_PWCTR2 0xC1 // Power control setting
#define ST7735S_PWCTR3 0xC2 // In normal mode (Full colors)
#define ST7735S_PWCTR4 0xC3 // In idle mode (8 colors)
#define ST7735S_PWCTR5 0xC4 // In partial mode + Full colors
#define ST7735S_VMCTR1 0xC5 // VCOM control 1
#define ST7735S_INVOFF 0x20 // Display inversion off
#define ST7735S_MADCTL 0x36 // Memory data access control
#define ST7735S_COLMOD 0x3A // Interface pixel format
#define ST7735S_CASET 0x2A
#define ST7735S_RASET 0x2B
#define ST7735S_GMCTRP1 0xE0
#define ST7735S_GMCTRN1 0xE1
#define ST7735S_NORON 0x13
#define ST7735S_DISPON 0x29
#define ST7735S_DISPOFF 0x28
#define ST7735S_RAMWR 0x2C

#define DELAY 0x80




struct Tst7735s_info {
	struct spi_device *spi;
	struct gpio_desc *dc_gpio;
	struct gpio_desc *reset_gpio;
};


static struct Tst7735s_info st7735s_info;
static u16 frame_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

////////////////////////////////////////////////////////////////////////////////

static const u8 init_cmds1[] = {
		// Init for st7735s, part 1 (red or green tab)
		15,// 15 commands in list:
		ST7735S_SWRESET, DELAY,//1: Software reset, 0 args, w/delay
		150,//150 ms delay
		ST7735S_SLPOUT, DELAY,// 2: Out of sleep mode, 0 args, w/delay
		255,//255 ms delay
		ST7735S_FRMCTR1, 3,//3: Frame rate ctrl - normal mode, 3 args:
		0x02, 0x2D, 0x2E,//Rate = fosc/(0x02+40) * (LINE+2D+2E)
		ST7735S_FRMCTR2, 3,//4: Frame rate control - idle mode, 3 args:
		0x02, 0x2D, 0x2E,//Rate = fosc/(0x02+40) * (LINE+2D+2E)
		ST7735S_FRMCTR3, 6,//5: Frame rate ctrl - partial mode, 6 args:
		0x02, 0x2D, 0x2E,//Dot inversion mode
		0x02, 0x2D, 0x2E,//Line inversion mode
		ST7735S_INVCTR, 1,// 6: Display inversion ctrl, 1 arg, no delay:
		0x07,//No inversion
		ST7735S_PWCTR1, 3,// 7: Power control, 3 args, no delay:
		0xA2,
		0x02,//-4.6V
		0x84,//AUTO mode
		ST7735S_PWCTR2, 1,// 8: Power control, 1 arg, no delay:
		0xC5,//VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
		ST7735S_PWCTR3, 2,// 9: Power control, 2 args, no delay:
		0x0A,//Opamp current small
		0x00,//Boost frequency
		ST7735S_PWCTR4, 2,//10: Power control, 2 args, no delay:
		0x8A,//BCLK/2, Opamp current small & Medium low
		0x2A,
		ST7735S_PWCTR5, 2,// 11: Power control, 2 args, no delay:
		0x8A, 0xEE,
		ST7735S_VMCTR1, 1,// 12:Power control, 1 arg, no delay:
		0x0E,
		ST7735S_INVOFF, 0,//13:Don't invert display, no args, no delay
		ST7735S_MADCTL, 1,//14:Memory access control (directions),1arg:
		ST7735S_ROTATION,//row addr/col addr, bottom to top refresh
		ST7735S_COLMOD, 1,//15: set color mode, 1 arg, no delay:
		0x05
}, //	16-bit color

init_cmds2[] = {
		// Init for st7735s, part 2
		2,//  2 commands in list:
		ST7735S_CASET, 4,//1: Column addr set, 4 args, no delay:
		0x00, 0x00,//XSTART = 0
		0x00, 0x7F,//XEND = 127
		ST7735S_RASET, 4,//2: Row addr set, 4 args, no delay:
		0x00, 0x00,//XSTART = 0
		0x00, 0x9F
},

init_cmds3[] = {
		//Init for st7735s, part 3
		4,//  4 commands in list:
		ST7735S_GMCTRP1, 16,//1:Magical unicorn dust, 16 args, no delay:
		0x02, 0x1c, 0x07, 0x12,
		0x37, 0x32, 0x29, 0x2d,
		0x29, 0x25, 0x2B, 0x39,
		0x00, 0x01, 0x03, 0x10,
		ST7735S_GMCTRN1, 16,//2:Sparkles and rainbows,16 args, no delay:
		0x03, 0x1d, 0x07, 0x06,
		0x2E, 0x2C, 0x29, 0x2D,
		0x2E, 0x2E, 0x37, 0x3F,
		0x00, 0x00, 0x02, 0x10,
		ST7735S_NORON, DELAY,//3:Normal display on, no args, w/delay
		10,//10 ms delay
		ST7735S_DISPON, DELAY,//4:Main screen turn on, no args w/delay
		100
};

static void st7735s_reset(void)
{
		//gpio_request(ST7735S_PIN_RESET, "ST7735S_PIN_RESET");
		//gpio_direction_output(ST7735S_PIN_RESET, 0);
		gpiod_set_value(st7735s_info.reset_gpio, 0);
		mdelay(5);
		gpiod_set_value(st7735s_info.reset_gpio, 1);
		//gpio_free(ST7735S_PIN_RESET);
}

static void st7735s_write_command(u8 cmd)
{
		gpiod_set_value(st7735s_info.dc_gpio, 0);
		spi_write(st7735s_info.spi, &cmd, sizeof(cmd));
}

static void st7735s_write_data(u8 *buff, size_t buff_size)
{
		gpiod_set_value(st7735s_info.dc_gpio, 1);
		spi_write(st7735s_info.spi, buff, buff_size);
}

static void st7735s_execute_command_list(const u8 *addr)
{
		u8 numCommands, numArgs;
		u16 ms;

		numCommands = *addr++;
		while (numCommands--) {
				u8 cmd = *addr++;
				st7735s_write_command(cmd);

				numArgs = *addr++;
				ms = numArgs & DELAY;
				numArgs &= ~DELAY;
				if (numArgs) {
						st7735s_write_data((u8 *)addr, numArgs);
						addr += numArgs;
				}

				if (ms) {
						ms = *addr++;
						if (ms == 255)
							ms = 500;
						mdelay(ms);
				}
	}
}

static void st7735s_set_address_window(u8 x0, u8 y0, u8 x1, u8 y1)
{
	u8 data[] = {0x00, x0 + ST7735S_XSTART, 0x00, x1 + ST7735S_XSTART};

	st7735s_write_command(ST7735S_CASET);
	st7735s_write_data(data, sizeof(data));

	// row address set
	st7735s_write_command(ST7735S_RASET);
	data[1] = y0 + ST7735S_YSTART;
	data[3] = y1 + ST7735S_YSTART;
	st7735s_write_data(data, sizeof(data));

	// write to RAM
	st7735s_write_command(ST7735S_RAMWR);
}

inline void st7735s_update_screen(void)
{
		st7735s_write_data((u8 *)frame_buffer, sizeof(u16) * ST7735S_WIDTH * ST7735S_HEIGHT);
}

void st7735s_draw_pixel(u16 x, u16 y, u16 color)
{
		if ((x >= ST7735S_WIDTH) || (y >= ST7735S_HEIGHT)) {
				goto out;
		}

		frame_buffer[x + ST7735S_WIDTH * y] = color;
		st7735s_update_screen();

		out:
				return;
}

void st7735s_fill_rectangle(u16 x, u16 y, u16 w, u16 h, u16 color)
{
		u16 i;
		u16 j;

		if ((x >= ST7735S_WIDTH) || (y >= ST7735S_HEIGHT)) {
				goto out;
		}

		if ((x + w - 1) > ST7735S_WIDTH) {
				w = ST7735S_WIDTH - x;
		}

		if ((y + h - 1) > ST7735S_HEIGHT) {
				h = ST7735S_HEIGHT - y;
		}

		for (j = 0; j < h; ++j) {
				for (i = 0; i < w; ++i) {
						frame_buffer[(x + ST7735S_WIDTH * y) + (i + ST7735S_WIDTH * j)] = color;
				}
		}

		st7735s_update_screen();

out:
		return;
}

void st7735s_fill_screen(u16 color)
{
		st7735s_fill_rectangle(0, 0, ST7735S_WIDTH, ST7735S_HEIGHT, color);
}

////////////////////////////////////////////////////////////////////////////////

static int st7735s_probe(struct spi_device *spi)
{
	int ret;
	u32 spi_frequency = 500000;
	struct device_node *np =  spi->dev.of_node;

	dev_info(&spi->dev, "%s:%d\n", __func__, __LINE__);

	if (of_get_property(np, "spi-max-frequency", NULL)) {

		of_property_read_u32(np, "spi-max-frequency", &spi_frequency);
	}

	spi->bits_per_word = 8;
	spi->mode = SPI_MODE_0;
	spi->max_speed_hz = spi_frequency;

	dev_info(&spi->dev, "%s:%d max_speed_hz:%d\n", __func__, __LINE__, spi->max_speed_hz);

	ret = spi_setup(spi);
	if (ret < 0)
		return ret;

	st7735s_info.spi = spi;

	st7735s_info.dc_gpio = gpiod_get(&spi->dev, "dc", GPIOD_OUT_LOW);
	if (IS_ERR(st7735s_info.dc_gpio)) {
			dev_info(&spi->dev, "%s:%d\n", __func__, __LINE__);
			st7735s_info.dc_gpio = NULL;
		}

	st7735s_info.reset_gpio = gpiod_get(&spi->dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(st7735s_info.reset_gpio)) {
		dev_info(&spi->dev, "%s:%d\n", __func__, __LINE__);
		st7735s_info.reset_gpio = NULL;
	}



	st7735s_reset();
	st7735s_execute_command_list(init_cmds1);
	st7735s_execute_command_list(init_cmds2);
	st7735s_execute_command_list(init_cmds3);
	pr_info("st7735s: device init completed\n");

	memset(frame_buffer, 0xFFFF, sizeof(frame_buffer));

	st7735s_set_address_window(0, 0, ST7735S_WIDTH - 1, ST7735S_HEIGHT - 1);

	//Examples

	st7735s_fill_screen(0x0000);

	st7735s_fill_rectangle(20, 130, 50, 20, 0x1111);
	st7735s_fill_rectangle(20, 55 + 60, 20, 60, 0x2222);
	st7735s_fill_rectangle(20 + 40, 55 + 60, 20, 60, 0x3333);
	st7735s_fill_rectangle(79, 80, 50, 60, 0x4444);
	st7735s_fill_rectangle(0, 0, 50, 60, 0x5555);

	dev_info(&spi->dev, "%s:%d\n", __func__, __LINE__);
	return 0;

}


static int st7735s_remove(struct spi_device *spi)
{
	gpiod_put(st7735s_info.dc_gpio);
	gpiod_put(st7735s_info.reset_gpio);
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
