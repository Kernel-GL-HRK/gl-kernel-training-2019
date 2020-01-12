#include "lcd_driver.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/fb.h>
#include <linux/uaccess.h>

#include "lcd_driver.h"

struct lcd_data {
	struct spi_device *spi;
	struct mutex mutex_sysfs;
	struct fb_info *info;
	u16 frame_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];
};

static struct lcd_data *lcd_st7735s_data;

static const u8 init_cmds1[] = {
	15,                     // 15 commands in list:
	ST7735S_SWRESET, DELAY, //  1: Software reset, 0 args, w/delay
	150,                    //     150 ms delay
	ST7735S_SLPOUT, DELAY,  //  2: Out of sleep mode, 0 args, w/delay
	255,                    //     255 ms delay
	ST7735S_FRMCTR1, 3,     //  3: Frame rate ctrl - normal mode, 3 args:
	0x02, 0x2D, 0x2E,       //     Rate = fosc/(0x02+40) * (LINE+2D+2E)
	ST7735S_FRMCTR2, 3,     //  4: Frame rate control - idle mode, 3 args:
	0x02, 0x2D, 0x2E,       //     Rate = fosc/(0x02+40) * (LINE+2D+2E)
	ST7735S_FRMCTR3, 6,     //  5: Frame rate ctrl - partial mode, 6 args:
	0x02, 0x2D, 0x2E,       //     Dot inversion mode
	0x02, 0x2D, 0x2E,       //     Line inversion mode
	ST7735S_INVCTR, 1,      //  6: Display inversion ctrl, 1 arg, no delay:
	0x07,                   //     No inversion
	ST7735S_PWCTR1, 3,      //  7: Power control, 3 args, no delay:
	0xA2,
	0x02,                   //     -4.6V
	0x84,                   //     AUTO mode
	ST7735S_PWCTR2, 1,      //  8: Power control, 1 arg, no delay:
	0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
	ST7735S_PWCTR3, 2,      //  9: Power control, 2 args, no delay:
	0x0A,                   //     Opamp current small
	0x00,                   //     Boost frequency
	ST7735S_PWCTR4, 2,      // 10: Power control, 2 args, no delay:
	0x8A,                   //     BCLK/2, Opamp current small & Medium low
	0x2A,
	ST7735S_PWCTR5, 2,      // 11: Power control, 2 args, no delay:
	0x8A, 0xEE,
	ST7735S_VMCTR1, 1,      // 12: Power control, 1 arg, no delay:
	0x0E,
	ST7735S_INVOFF, 0,      // 13: Don't invert display, no args, no delay
	ST7735S_MADCTL, 1,  // 14: Memory access control (directions), 1 arg:
	ST7735S_ROTATION,       //     row addr/col addr, bottom to top refresh
	ST7735S_COLMOD, 1,      // 15: set color mode, 1 arg, no delay:
	0x05
}, //     16-bit color

init_cmds2[] = {
	2,                      //  2 commands in list:
	ST7735S_CASET, 4,       //  1: Column addr set, 4 args, no delay:
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F,             //     XEND = 127
	ST7735S_RASET, 4,       //  2: Row addr set, 4 args, no delay:
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x9F
},

init_cmds3[] = {
	4,                      //  4 commands in list:
	ST7735S_GMCTRP1, 16,    //  1: Magical unicorn dust, 16 args, no delay:
	0x02, 0x1c, 0x07, 0x12,
	0x37, 0x32, 0x29, 0x2d,
	0x29, 0x25, 0x2B, 0x39,
	0x00, 0x01, 0x03, 0x10,
	ST7735S_GMCTRN1, 16,  //  2: Sparkles and rainbows, 16 args, no delay:
	0x03, 0x1d, 0x07, 0x06,
	0x2E, 0x2C, 0x29, 0x2D,
	0x2E, 0x2E, 0x37, 0x3F,
	0x00, 0x00, 0x02, 0x10,
	ST7735S_NORON, DELAY,    //  3: Normal display on, no args, w/delay
	10,                      //     10 ms delay
	ST7735S_DISPON, DELAY,   //  4: Main screen turn on, no args w/delay
	100
};

static void lcd_reset(void)
{
	gpio_request(ST7735S_PIN_RESET, "ST7735S_PIN_RESET");
	gpio_direction_output(ST7735S_PIN_RESET, 0);
	gpio_set_value(ST7735S_PIN_RESET, 0);
	mdelay(5);
	gpio_set_value(ST7735S_PIN_RESET, 1);
	gpio_free(ST7735S_PIN_RESET);
}

static void lcd_write_command(u8 cmd)
{
	gpio_set_value(ST7735S_PIN_DC, 0);
	spi_write(lcd_st7735s_data->spi, &cmd, sizeof(cmd));
}

static void lcd_write_data(u8 *buff, size_t buff_size)
{
	gpio_set_value(ST7735S_PIN_DC, 1);
	spi_write(lcd_st7735s_data->spi, buff, buff_size);
}

static void lcd_cmd_list(const u8 *addr)
{
	u8 numCommands, numArgs;
	u16 ms;

	numCommands = *addr++;
	while (numCommands--) {
		u8 cmd = *addr++;

		lcd_write_command(cmd);

		numArgs = *addr++;
		ms = numArgs & DELAY;
		numArgs &= ~DELAY;
		if (numArgs) {
			lcd_write_data((u8 *)addr, numArgs);
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

static void lcd_set_adr_window(u8 x0, u8 y0, u8 x1, u8 y1)
{
	u8 data[] = {0x00, x0 + ST7735S_XSTART, 0x00, x1 + ST7735S_XSTART};

	lcd_write_command(ST7735S_CASET);
	lcd_write_data(data, sizeof(data));

	// row address set
	lcd_write_command(ST7735S_RASET);
	data[1] = y0 + ST7735S_YSTART;
	data[3] = y1 + ST7735S_YSTART;
	lcd_write_data(data, sizeof(data));

	// write to RAM
	lcd_write_command(ST7735S_RAMWR);
}

inline void lcd_update_screen(void)
{
	lcd_write_data((u8 *)lcd_st7735s_data->frame_buffer,
			sizeof(u16) * ST7735S_WIDTH * ST7735S_HEIGHT);
}

void lcd_draw_pixel(u16 x, u16 y, u16 color)
{
	if ((x >= ST7735S_WIDTH) || (y >= ST7735S_HEIGHT))
			goto out;

	lcd_st7735s_data->frame_buffer[x + ST7735S_WIDTH * y] = color;
	lcd_update_screen();

out:
	return;
}

void lcd_fill_rect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
	u16 i;
	u16 j;

	if ((x >= ST7735S_WIDTH) || (y >= ST7735S_HEIGHT))
			goto out;

	if ((x + w - 1) > ST7735S_WIDTH)
			w = ST7735S_WIDTH - x;

	if ((y + h - 1) > ST7735S_HEIGHT)
			h = ST7735S_HEIGHT - y;

	for (j = 0; j < h; ++j) {
			for (i = 0; i < w; ++i) {
				lcd_st7735s_data->frame_buffer[(x + ST7735S_WIDTH * y) +
					(i + ST7735S_WIDTH * j)] = color;
			}
	}

	lcd_update_screen();

out:
	return;
}

void lcd_fill_screen(u16 color)
{
	lcd_fill_rect(0, 0, ST7735S_WIDTH, ST7735S_HEIGHT, color);
}

static int st7735s_probe(struct spi_device *spi)
{
	lcd_st7735s_data = devm_kzalloc(&spi->dev, sizeof(struct lcd_data),
	GFP_KERNEL);

		if (!lcd_st7735s_data)
			return -ENOMEM;

	spi_set_drvdata(spi, lcd_st7735s_data);
	lcd_st7735s_data->spi = spi;

	mutex_init(&lcd_st7735s_data->mutex_sysfs);

	gpio_request(ST7735S_PIN_DC, "ST7735S_PIN_DC");
	gpio_direction_output(ST7735S_PIN_DC, 0);
	lcd_reset();
	lcd_cmd_list(init_cmds1);
	lcd_cmd_list(init_cmds2);
	lcd_cmd_list(init_cmds3);
	pr_info("lcd: device init completed\n");

	memset(lcd_st7735s_data->frame_buffer, 0xFFFF,
			sizeof(lcd_st7735s_data->frame_buffer));

	lcd_set_adr_window(0, 0, ST7735S_WIDTH - 1, ST7735S_HEIGHT - 1);

	//Examples
	lcd_fill_screen(0x0000);
	lcd_fill_rect(34, 0, 60, 40, 0x03e0);
	lcd_fill_rect(34, 60, 60, 40, 0x001f);
	lcd_fill_rect(34, 120, 60, 40, 0xf800);

	return 0;
}

static int st7735s_remove(struct spi_device *spi)
{
	lcd_st7735s_data = spi_get_drvdata(spi);

	if (!lcd_st7735s_data)
		return -ENODEV;

	return 0;
}

static const struct of_device_id st7735s_idtable[] = {
	{ .compatible = "st7735s", },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, st7735s_idtable);

static struct spi_driver st7735s_spi_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "st7735s",
		.of_match_table = of_match_ptr(st7735s_idtable),
	},

	.probe = st7735s_probe,
	.remove = st7735s_remove,
};

static ssize_t fill_screen_store(struct class *class,
		struct class_attribute *attr, const char *buf, size_t size)
{
	u16 color = 0x0000;

	mutex_lock(&lcd_st7735s_data->mutex_sysfs);
	sscanf(buf, "%hx", &color);
	lcd_fill_screen(color);
	mutex_unlock(&lcd_st7735s_data->mutex_sysfs);

	return size;
}

static ssize_t fill_rect_store(struct class *class,
		struct class_attribute *attr, const char *buf, size_t size)
{
	u16 color = 0xf0f0;
	int x;
	int y;
	int w;
	int h;

	mutex_lock(&lcd_st7735s_data->mutex_sysfs);
	sscanf(buf, "%hx %d %d %d %d", &color, &x, &y, &w, &h);
	lcd_fill_rect(x, y, w, h, color);
	mutex_unlock(&lcd_st7735s_data->mutex_sysfs);

	return size;
}

CLASS_ATTR_WO(fill_screen);
CLASS_ATTR_WO(fill_rect);

static struct class *attr_class;

static int st7735s_init(void)
{
	int ret;

	spi_register_driver(&st7735s_spi_driver);

	/* Create class */
	attr_class = class_create(THIS_MODULE, "lcd_st7735s");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		pr_err("lcd: failed to create sysfs class: %d\n", ret);
		return ret;
	}
	pr_info("lcd: sysfs class created\n");

	/* Create fill_screen */
	ret = class_create_file(attr_class, &class_attr_fill_screen);
	if (ret) {
		pr_err("lcd: failed to create sysfs class attribute accel_x:%d\n",
				ret);
		return ret;
	}
	/* Create fill_rect */
	ret = class_create_file(attr_class, &class_attr_fill_rect);
	if (ret) {
		pr_err("lcd: failed to create sysfs class attribute accel_y:%d\n",
				ret);
		return ret;
	}

	pr_info("lcd: module loaded\n");
	return 0;
}

static void st7735s_exit(void)
{
	if (attr_class) {
		class_remove_file(attr_class, &class_attr_fill_screen);
		class_remove_file(attr_class, &class_attr_fill_rect);
		pr_info("lcd: sysfs class attributes removed\n");

		class_destroy(attr_class);
		pr_info("lcd: sysfs class destroyed\n");
	}

	spi_unregister_driver(&st7735s_spi_driver);
	pr_info("lcd: module exited\n");
}

module_init(st7735s_init);
module_exit(st7735s_exit);

MODULE_AUTHOR("Bekir.Bekirov <bekirbekirov1986@gmail.com>");
MODULE_DESCRIPTION("lcd module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
