#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/device.h>

#include "st7735s_types.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aaaaaaaaaaaaaaaaaa");
MODULE_DESCRIPTION("Driver for st7735 display");

#define DEVICE_NAME    "st7735s_display"
#define CLASS_NAME     "st7735s_display" 
#define BUS_NAME       "spi0"

static u16 frame_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];
static struct spi_device *st7735s_spi_device;

static const u8 init_cmds1[] = { 
        // Init for st7735s, part 1 (red or green tab)
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
        ST7735S_MADCTL, 1,      // 14: Memory access control (directions), 1 arg:
        ST7735S_ROTATION,       //     row addr/col addr, bottom to top refresh
        ST7735S_COLMOD, 1,      // 15: set color mode, 1 arg, no delay:
        0x05
}, //     16-bit color

init_cmds2[] = {
        // Init for st7735s, part 2
        2,                      //  2 commands in list:
        ST7735S_CASET, 4,       //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,             //     XSTART = 0
        0x00, 0x7F,             //     XEND = 127
        ST7735S_RASET, 4,       //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,             //     XSTART = 0
        0x00, 0x9F
},

init_cmds3[] = { 
        // Init for st7735s, part 3
        4,                      //  4 commands in list:
        ST7735S_GMCTRP1, 16,    //  1: Magical unicorn dust, 16 args, no delay:
        0x02, 0x1c, 0x07, 0x12,
        0x37, 0x32, 0x29, 0x2d,
        0x29, 0x25, 0x2B, 0x39,
        0x00, 0x01, 0x03, 0x10,
        ST7735S_GMCTRN1, 16,     //  2: Sparkles and rainbows, 16 args, no delay:
        0x03, 0x1d, 0x07, 0x06,
        0x2E, 0x2C, 0x29, 0x2D,
        0x2E, 0x2E, 0x37, 0x3F,
        0x00, 0x00, 0x02, 0x10,
        ST7735S_NORON, DELAY,    //  3: Normal display on, no args, w/delay
        10,                      //     10 ms delay
        ST7735S_DISPON, DELAY,   //  4: Main screen turn on, no args w/delay
        100
};

static void reset(void)
{
        gpio_request(ST7735S_PIN_RESET, "ST7735S_PIN_RESET");
        gpio_direction_output(ST7735S_PIN_RESET, 0);
        gpio_set_value(ST7735S_PIN_RESET, 0);
        mdelay(5);
        gpio_set_value(ST7735S_PIN_RESET, 1);
        gpio_free(ST7735S_PIN_RESET);
}


static void write_data(u8 *buff, size_t buff_size)
{
        gpio_set_value(ST7735S_PIN_DC, 1);
        spi_write(st7735s_spi_device, buff, buff_size);
}

static void write_command(u8 cmd)
{
        gpio_set_value(ST7735S_PIN_DC, 0);
        spi_write(st7735s_spi_device, &cmd, sizeof(cmd));
}

static void execute_command_list(const u8 *addr)
{
        u8 numCommands, numArgs;
        u16 ms;

        numCommands = *addr++;
        while (numCommands--) {
                u8 cmd = *addr++;
                write_command(cmd);

                numArgs = *addr++;
                ms = numArgs & DELAY;
                numArgs &= ~DELAY;
                if (numArgs) {
                        write_data((u8*)addr, numArgs);
                        addr += numArgs;
                }

                if (ms) {
                        ms = *addr++;
                        if (ms == 255) ms = 500;
                        mdelay(ms);
                }
    }
}


static inline void update_screen(void)
{
        write_data((u8*)frame_buffer, sizeof(u16) * ST7735S_WIDTH * ST7735S_HEIGHT);
}


static void fill_rectangle(u16 x, u16 y, u16 w, u16 h, u16 color)
{
        u16 i;
        u16 j;

        if ((x >= ST7735S_WIDTH) || (y >= ST7735S_HEIGHT)) {
                return;
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

        update_screen();
}

static void fill_screen(u16 color)
{
        fill_rectangle(0, 0, ST7735S_WIDTH, ST7735S_HEIGHT, color);
}


static ssize_t draw_rect_store(struct class *class, struct class_attribute *attr, const char *buf, size_t size)
{
        u16 color = 0xf0f0;
        int x;
        int y;
        int w;
        int h;

        sscanf(buf, "%hx %d %d %d %d", &color, &x, &y, &w, &h);
                fill_rectangle(x, y, w, h, color);

        return size;
}

static ssize_t fill_screen_store(struct class *class, struct class_attribute *attr, const char *buf, size_t size)
{
        u16 color = 0x0000;

        sscanf(buf, "%hx", &color);

        fill_screen(color);

        return size;
}

static void set_address_window(u8 x0, u8 y0, u8 x1, u8 y1)
{
    u8 data[] = {0x00, x0 + ST7735S_XSTART, 0x00, x1 + ST7735S_XSTART};

    write_command(ST7735S_CASET);
    write_data(data, sizeof(data));

    // row address set
    write_command(ST7735S_RASET);
    data[1] = y0 + ST7735S_YSTART;
    data[3] = y1 + ST7735S_YSTART;
    write_data(data, sizeof(data));

    // write to RAM
    write_command(ST7735S_RAMWR);
}


static struct class *attr_class;
static struct class_attribute class_attr_draw_rect = __ATTR_WO(draw_rect);
static struct class_attribute class_attr_fill_screen = __ATTR_WO(fill_screen);

static void __exit st7735s_exit(void)
{
    if (attr_class) {
        class_remove_file(attr_class, &class_attr_draw_rect);
        class_remove_file(attr_class, &class_attr_fill_screen);
        pr_info("st7735s: sysfs class attributes removed\n");

        class_destroy(attr_class);
        pr_info("st7735s: sysfs class destroyed\n");
    }

    if (st7735s_spi_device) {
            spi_unregister_device(st7735s_spi_device);
    }
    pr_info("st7735s: spi device unregistered\n");

    pr_info("st7735s: module exited\n");
}

static int __init st7735s_init(void)
{
        int ret;
        struct spi_master *master;

        //Register information about your slave device:
        struct spi_board_info st7735s_info = {
                .modalias = "st7735s",
                .max_speed_hz = 100000, //speed your device (slave) can handle
                .bus_num = 0,
                .chip_select = 0,
                .mode = SPI_MODE_0,
        };

        master = spi_busnum_to_master(st7735s_info.bus_num);
        if (!master) {
                printk("MASTER not found.\n");
                ret = -ENODEV;
                goto out;
        }

        // create a new slave device, given the master and device info
        st7735s_spi_device = spi_new_device(master, &st7735s_info);
        if (!st7735s_spi_device) {
                printk("FAILED to create slave.\n");
                ret = -ENODEV;
                goto out;
        }

        st7735s_spi_device->bits_per_word = 8,

        ret = spi_setup(st7735s_spi_device);
        if (ret) {
                printk("FAILED to setup slave.\n");
                spi_unregister_device(st7735s_spi_device);
                ret = -ENODEV;
                goto out;
        }

        pr_info("st7735s: spi device setup completed\n");

        gpio_request(ST7735S_PIN_DC, "ST7735S_PIN_DC");
        gpio_direction_output(ST7735S_PIN_DC, 0);
        reset();
        execute_command_list(init_cmds1);
        execute_command_list(init_cmds2);
        execute_command_list(init_cmds3);
        pr_info("st7735s: device init completed\n");

        memset(frame_buffer, 0xFFFF, sizeof(frame_buffer));

        set_address_window(0, 0, ST7735S_WIDTH - 1, ST7735S_HEIGHT - 1);

        // create sysfs
        attr_class = class_create(THIS_MODULE, DEVICE_NAME);
        if (IS_ERR(attr_class)) {
                ret = PTR_ERR(attr_class);
                pr_err("st7735s: failed to create sysfs class: %d\n", ret);
                goto out;
        }
        pr_info("st7735s: sysfs class created\n");

        class_attr_fill_screen.attr.mode = 0222;
        ret = class_create_file(attr_class, &class_attr_fill_screen);
        if (ret) {
                pr_err("st7735s: failed to created sysfs class attribute direction: %d\n", ret);
                goto out;
        }
        
        class_attr_draw_rect.attr.mode = 0222;
        ret = class_create_file(attr_class, &class_attr_draw_rect);
        if (ret) {
                pr_err("st7735s: failed to created sysfs class attribute direction: %d\n", ret);
                goto out;
        }

        pr_info("st7735s: sysfs class attributes created\n");

        pr_info("st7735s: module loaded\n");

        return 0;

out:
        return ret;
}

module_init(st7735s_init);
module_exit(st7735s_exit);
