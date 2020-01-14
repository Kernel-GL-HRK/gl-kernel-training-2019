#ifndef ST7735S_TYPES_H
#define ST7735S_TYPES_H

#include <linux/spi/spi.h>
#include <linux/mutex.h>
#include <linux/fb.h>

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

#define ST7735S_DEVICE_NAME "st7735s"

struct lcd_data {
	struct spi_device *spi;
	struct mutex mutex_sysfs;
	struct fb_info *info;
	u16 frame_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];
	u32 height;
	u32 width;
};

void lcd_update_screen(void);

#endif
