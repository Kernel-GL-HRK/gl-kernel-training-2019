#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>

#include "st7735s_image.h"

#define ST7735S_MADCTL_RGB	0x00
#define ST7735S_MADCTL_BGR	0x08
#define ST7735S_MADCTL_MY	0x80
#define ST7735S_MADCTL_MX	0x40

#define ST7735S_IS_160X128	1
#define ST7735S_WIDTH		128
#define ST7735S_HEIGHT		160
#define ST7735S_XSTART		0
#define ST7735S_YSTART		0
#define ST7735S_ROTATION	(ST7735S_MADCTL_MX | ST7735S_MADCTL_MY)

// Commands
#define ST7735S_SWRESET		0x01 // SoftWare Reset
#define ST7735S_SLPOUT		0x11 // Sleep Out
#define ST7735S_FRMCTR1		0xB1 // Norman Mode (Full colors)
#define ST7735S_FRMCTR2		0xB2 // In idle Mode (8 colors)
#define ST7735S_FRMCTR3		0xB3 // In partial Mode + Full Colors
#define ST7735S_INVCTR		0xB4 // Display inversion control
#define ST7735S_PWCTR1		0xC0 // Power control setting
#define ST7735S_PWCTR2		0xC1 // Power control setting
#define ST7735S_PWCTR3		0xC2 // In normal mode (Full colors)
#define ST7735S_PWCTR4		0xC3 // In idle mode (8 colors)
#define ST7735S_PWCTR5		0xC4 // In partial mode + Full colors
#define ST7735S_VMCTR1		0xC5 // VCOM control 1
#define ST7735S_INVOFF		0x20 // Display inversion off
#define ST7735S_MADCTL		0x36 // Memory data access control
#define ST7735S_COLMOD		0x3A // Interface pixel format
#define ST7735S_CASET		0x2A
#define ST7735S_RASET		0x2B
#define ST7735S_GMCTRP1		0xE0
#define ST7735S_GMCTRN1		0xE1
#define ST7735S_NORON		0x13
#define ST7735S_DISPON		0x29
#define ST7735S_DISPOFF		0x28
#define ST7735S_RAMWR		0x2C

#define DELAY			0x80

static const u8 init_cmds1[] = {
	// Init for st7735s, part 1 (red or green tab)
	15,			// 15 commands in list:
	ST7735S_SWRESET, DELAY,	// 1: Software reset, 0 args, w/delay
	150,			// 150 ms delay
	ST7735S_SLPOUT, DELAY,	// 2: Out of sleep mode, 0 args, w/delay
	255,			// 255 ms delay
	ST7735S_FRMCTR1, 3,	// 3: Frame rate ctrl - normal mode, 3 args:
	0x02, 0x2D, 0x2E,	// Rate = fosc/(0x02+40) * (LINE+2D+2E)
	ST7735S_FRMCTR2, 3,	// 4: Frame rate control - idle mode, 3 args:
	0x02, 0x2D, 0x2E,	// Rate = fosc/(0x02+40) * (LINE+2D+2E)
	ST7735S_FRMCTR3, 6,	// 5: Frame rate ctrl - partial mode, 6 args:
	0x02, 0x2D, 0x2E,	// Dot inversion mode
	0x02, 0x2D, 0x2E,	// Line inversion mode
	ST7735S_INVCTR, 1,	// 6: Display inversion ctrl, 1 arg, no delay:
	0x07,			// No inversion
	ST7735S_PWCTR1, 3,	// 7: Power control, 3 args, no delay:
	0xA2,
	0x02,			// -4.6V
	0x84,			// AUTO mode
	ST7735S_PWCTR2, 1,	// 8: Power control, 1 arg, no delay:
	0xC5,			// VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
	ST7735S_PWCTR3, 2,	// 9: Power control, 2 args, no delay:
	0x0A,			// Opamp current small
	0x00,			// Boost frequency
	ST7735S_PWCTR4, 2,	// 10: Power control, 2 args, no delay:
	0x8A,			// BCLK/2, Opamp current small & Medium low
	0x2A,
	ST7735S_PWCTR5, 2,	// 11: Power control, 2 args, no delay:
	0x8A, 0xEE,
	ST7735S_VMCTR1, 1,	// 12: Power control, 1 arg, no delay:
	0x0E,
	ST7735S_INVOFF, 0,	// 13: Don't invert display, no args, no delay
	ST7735S_MADCTL, 1, // 14: Memory access control (directions), 1 arg:
	ST7735S_ROTATION,	// row addr/col addr, bottom to top refresh
	ST7735S_COLMOD, 1,	// 15: set color mode, 1 arg, no delay:
	0x05
}; // 16-bit color

static const u8 init_cmds2[] = {
	// Init for st7735s, part 2
	2,			// 2 commands in list:
	ST7735S_CASET, 4,	// 1: Column addr set, 4 args, no delay:
	0x00, 0x00,		// XSTART = 0
	0x00, 0x7F,		// XEND = 127
	ST7735S_RASET, 4,	// 2: Row addr set, 4 args, no delay:
	0x00, 0x00,		// XSTART = 0
	0x00, 0x9F
};

static const u8 init_cmds3[] = {
	// Init for st7735s, part 3
	4,			// 4 commands in list:
	ST7735S_GMCTRP1, 16,	// 1: Magical unicorn dust, 16 args, no delay:
	0x02, 0x1c, 0x07, 0x12,
	0x37, 0x32, 0x29, 0x2d,
	0x29, 0x25, 0x2B, 0x39,
	0x00, 0x01, 0x03, 0x10,
	ST7735S_GMCTRN1, 16,	// 2: Sparkles and rainbows, 16 args, no delay:
	0x03, 0x1d, 0x07, 0x06,
	0x2E, 0x2C, 0x29, 0x2D,
	0x2E, 0x2E, 0x37, 0x3F,
	0x00, 0x00, 0x02, 0x10,
	ST7735S_NORON, DELAY,	// 3: Normal display on, no args, w/delay
	10,			// 10 ms delay
	ST7735S_DISPON, DELAY,	// 4: Main screen turn on, no args w/delay
	100
};

struct st7735s {
	struct spi_device *spi;
	struct gpio_desc *gpiod_reset;
	struct gpio_desc *gpiod_a0;
	u16 frame_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];
	struct class lcd_rc_class;
};

static void st7735s_reset(struct st7735s *lcd)
{
	gpiod_set_value(lcd->gpiod_reset, 0);
	mdelay(5);
	gpiod_set_value(lcd->gpiod_reset, 1);
}

static void st7735s_write_command(struct st7735s *lcd, u8 cmd)
{
	gpiod_set_value(lcd->gpiod_a0, 0);
	spi_write(lcd->spi, &cmd, sizeof(cmd));
}

static void st7735s_write_data(struct st7735s *lcd, u8 *buff, size_t buff_size)
{
	gpiod_set_value(lcd->gpiod_a0, 1);
	spi_write(lcd->spi, buff, buff_size);
}

static void st7735s_execute_command_list(struct st7735s *lcd, const u8 *addr)
{
	u8 numCommands, numArgs;
	u16 ms;

	numCommands = *addr++;
	while (numCommands--) {
		u8 cmd = *addr++;

		st7735s_write_command(lcd, cmd);
		numArgs = *addr++;
		ms = numArgs & DELAY;
		numArgs &= ~DELAY;
		if (numArgs) {
			st7735s_write_data(lcd, (u8 *)addr, numArgs);
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

static void st7735s_set_address_window(struct st7735s *lcd, u8 x0, u8 y0,
	u8 x1, u8 y1)
{
	u8 data[] = {0x00, x0 + ST7735S_XSTART, 0x00, x1 + ST7735S_XSTART};

	st7735s_write_command(lcd, ST7735S_CASET);
	st7735s_write_data(lcd, data, sizeof(data));

	// row address set
	st7735s_write_command(lcd, ST7735S_RASET);
	data[1] = y0 + ST7735S_YSTART;
	data[3] = y1 + ST7735S_YSTART;
	st7735s_write_data(lcd, data, sizeof(data));

	// write to RAM
	st7735s_write_command(lcd, ST7735S_RAMWR);
}

static u16 st7735s_conversion_color(u16 color)
{
	u16 c = 0;

	c = (color & 0x001F) << 8; // Blue
	c |= (color & 0xF800) >> 8; // Red
	c |= (color & 0x0700) >> 8; // Green 1
	c |= (color & 0x00E0) << 8; // Green 2

	return c;
}

inline void st7735s_update_screen(struct st7735s *lcd)
{
	st7735s_write_data(lcd, (u8 *)lcd->frame_buffer,
		sizeof(u16) * ST7735S_WIDTH * ST7735S_HEIGHT);
}

void st7735s_load_image(struct st7735s *lcd, const u8 *image)
{
	int pix = 0;

	for (pix = 0; pix < ST7735S_WIDTH * ST7735S_HEIGHT; pix++) {
		lcd->frame_buffer[pix] =
				st7735s_conversion_color(*(u16 *)image);
		image += 2;
	}

	st7735s_update_screen(lcd);
}

void st7735s_fill_rectangle(struct st7735s *lcd, u16 x, u16 y, u16 w, u16 h,
						u16 color)
{
	u16 i = 0;
	u16 j = 0;

	if ((x >= ST7735S_WIDTH) || (y >= ST7735S_HEIGHT))
		return;

	if ((x + w - 1) > ST7735S_WIDTH)
		w = ST7735S_WIDTH - x;

	if ((y + h - 1) > ST7735S_HEIGHT)
		h = ST7735S_HEIGHT - y;

	for (j = 0; j < h; ++j)
		for (i = 0; i < w; ++i) {
			lcd->frame_buffer[(x + ST7735S_WIDTH * y) +
		(i + ST7735S_WIDTH * j)] = st7735s_conversion_color(color);
		}

	st7735s_update_screen(lcd);
}

static ssize_t draw_rect_store(struct class *class,
			 struct class_attribute *attr,
			 const char *buf, size_t count)
{
	struct st7735s *lcd = container_of(class, struct st7735s, lcd_rc_class);
	ssize_t result = 0;
	u16 color = 0x0000;
	int x = 0, y = 0, w = 0, h = 0;

	result = sscanf(buf, "%hx %d %d %d %d", &color, &x, &y, &w, &h);
	if (result != 5)
		return -EINVAL;

	st7735s_fill_rectangle(lcd, x, y, w, h, color);
	st7735s_update_screen(lcd);

	return count;
}

static ssize_t fill_screen_store(struct class *class,
			 struct class_attribute *attr,
			 const char *buf, size_t count)
{
	struct st7735s *lcd = container_of(class, struct st7735s, lcd_rc_class);
	ssize_t result = 0;
	u16 color = 0x0000;

	result = sscanf(buf, "%hx", &color);
	if (result != 1)
		return -EINVAL;

	st7735s_fill_rectangle(lcd, 0, 0, ST7735S_WIDTH,
			ST7735S_HEIGHT, color);
	st7735s_update_screen(lcd);

	return count;
}

CLASS_ATTR_WO(draw_rect);
CLASS_ATTR_WO(fill_screen);

static struct attribute *lcd_class_attrs[] = {
	&class_attr_draw_rect.attr,
	&class_attr_fill_screen.attr,
	NULL,
};

ATTRIBUTE_GROUPS(lcd_class);

static int st7735s_probe(struct spi_device *spi)
{
	struct st7735s *lcd;
	int ret = 0;
	struct gpio_desc *gpiod;

	lcd = devm_kzalloc(&spi->dev, sizeof(*lcd), GFP_KERNEL);
	if (IS_ERR(lcd)) {
		dev_err(&spi->dev, "error mem <devm_kzalloc>\n");
		return -ENOMEM;
	}

	/* Get the Reset GPIO pin number */
	gpiod = devm_gpiod_get_optional(&spi->dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(gpiod)) {
		ret = PTR_ERR(gpiod);
		if (ret != -EPROBE_DEFER)
			dev_err(&spi->dev, "Failed to get %s GPIO: %d\n",
				"reset", ret);
		devm_kfree(&spi->dev, lcd);
		return ret;
	}
	lcd->gpiod_reset = gpiod;
	ret = gpiod_direction_output(lcd->gpiod_reset, 0);

	/* Get the A0 GPIO pin number */
	gpiod = devm_gpiod_get_optional(&spi->dev, "a0", GPIOD_OUT_LOW);
	if (IS_ERR(gpiod)) {
		ret = PTR_ERR(gpiod);
		if (ret != -EPROBE_DEFER)
			dev_err(&spi->dev, "Failed to get %s GPIO: %d\n",
				"a0", ret);
		gpiod_put(lcd->gpiod_reset);
		devm_kfree(&spi->dev, lcd);
		return ret;
	}
	lcd->gpiod_a0 = gpiod;
	gpiod_direction_output(lcd->gpiod_a0, 0);

	strcpy(spi->modalias, "st7735s");
	spi->max_speed_hz = 25e6; //speed your device (slave) can handle
	spi->chip_select = 0;
	spi->mode = SPI_MODE_0;
	ret = spi_setup(spi);
	if (ret < 0) {
		dev_err(&spi->dev, "failed to setup spi: %d\n", ret);
		gpiod_put(lcd->gpiod_reset);
		gpiod_put(lcd->gpiod_a0);
		devm_kfree(&spi->dev, lcd);
		return ret;
	}

	lcd->spi = spi;

	st7735s_reset(lcd);
	st7735s_execute_command_list(lcd, init_cmds1);
	st7735s_execute_command_list(lcd, init_cmds2);
	st7735s_execute_command_list(lcd, init_cmds3);
	st7735s_set_address_window(lcd, 0, 0, ST7735S_WIDTH - 1,
						ST7735S_HEIGHT - 1);
	dev_info(&spi->dev, "device init completed\n");

	memset(lcd->frame_buffer, 0xFFFF, sizeof(lcd->frame_buffer));

	// 128*160
	// Example x-y-w-h
	//st7735s_fill_rectangle(lcd, 0, 0, ST7735S_WIDTH,
	//					ST7735S_HEIGHT, 0x07A5);
	//st7735s_fill_rectangle(lcd, 0, 0, 30, 30, 0xF0DD);
	//st7735s_fill_rectangle(lcd, 98, 0, 30, 30, 0xF1A0);
	//st7735s_fill_rectangle(lcd, 98, 130, 30, 30, 0xEF83);
	//st7735s_fill_rectangle(lcd, 0, 130, 30, 30, 0x18DE);

	// Test load image
	st7735s_load_image(lcd, lcd_image);

	/* Device model classes */
	lcd->lcd_rc_class.name = "st7735s";
	lcd->lcd_rc_class.owner = THIS_MODULE;
	lcd->lcd_rc_class.class_groups = lcd_class_groups;

	ret = class_register(&lcd->lcd_rc_class);
	if (ret < 0) {
		dev_err(&spi->dev, "failed to create sysfs class: %d\n", ret);
		gpiod_put(lcd->gpiod_reset);
		gpiod_put(lcd->gpiod_a0);
		devm_kfree(&spi->dev, lcd);
		return ret;
	}
	dev_info(&spi->dev, "sysfs class created\n");

	dev_set_drvdata(&spi->dev, lcd);

	dev_info(&spi->dev, "spi driver probed\n");
	return 0;
}

static int st7735s_remove(struct spi_device *spi)
{
	struct st7735s *lcd = dev_get_drvdata(&spi->dev);

	class_unregister(&lcd->lcd_rc_class);
	dev_info(&spi->dev, "sysfs class destroyed\n");

	st7735s_write_command(lcd, ST7735S_DISPOFF);

	gpiod_put(lcd->gpiod_reset);
	gpiod_put(lcd->gpiod_a0);

	devm_kfree(&spi->dev, lcd);

	dev_info(&spi->dev, "spi driver removed\n");
	return 0;
}

static const struct of_device_id st7735s_ids[] = {
	{ .compatible = "qd, st7735s", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, st7735s_ids);

static const struct spi_device_id st7735s_idtable[] = {
	{ "st7735s", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, st7735s_idtable);

static struct spi_driver st7735s_spi_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "st7735s",
		.of_match_table = of_match_ptr(st7735s_ids),
	},
	.probe = st7735s_probe,
	.remove = st7735s_remove,
	.id_table = st7735s_idtable,
};

module_spi_driver(st7735s_spi_driver);

MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("ST7735S spi lcd module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
