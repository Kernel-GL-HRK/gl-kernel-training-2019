#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <getopt.h>

struct fb_config {
	int fd;
	int width;
	int height;
	int height_virtual;
	int bpp;
	int stride;
	int red_offset;
	int red_length;
	int green_offset;
	int green_length;
	int blue_offset;
	int blue_length;
	int transp_offset;
	int transp_length;
	int buffer_num;
	short *data;
	char *base;
};

void dump_vscreeninfo(struct fb_var_screeninfo *fvsi)
{
	printf("======= FB VAR SCREENINFO =======\n");
	printf("xres: %d\n", fvsi->xres);
	printf("yres: %d\n", fvsi->yres);
	printf("yres_virtual: %d\n", fvsi->yres_virtual);
	printf("buffer number: %d\n", fvsi->yres_virtual / fvsi->yres);
	printf("bpp : %d\n", fvsi->bits_per_pixel);
	printf("red bits    :\n");
	printf("    offset   : %d\n", fvsi->red.offset);
	printf("    length   : %d\n", fvsi->red.length);
	printf("    msb_right: %d\n", fvsi->red.msb_right);
	printf("green bits  :\n");
	printf("    offset   : %d\n", fvsi->green.offset);
	printf("    length   : %d\n", fvsi->green.length);
	printf("    msb_right: %d\n", fvsi->green.msb_right);
	printf("blue bits   :\n");
	printf("    offset   : %d\n", fvsi->blue.offset);
	printf("    length   : %d\n", fvsi->blue.length);
	printf("    msb_right: %d\n", fvsi->blue.msb_right);
	printf("transp bits :\n");
	printf("    offset   : %d\n", fvsi->transp.offset);
	printf("    length   : %d\n", fvsi->transp.length);
	printf("    msb_right: %d\n", fvsi->transp.msb_right);

	printf("=================================\n");
}

void dump_fscreeninfo(struct fb_fix_screeninfo *ffsi)
{
	printf("======= FB FIX SCREENINFO =======\n");
	printf("id          : %s\n", ffsi->id);
	printf("smem_start  : 0x%08lX\n", ffsi->smem_start);
	printf("smem_len    : %u\n", ffsi->smem_len);
	printf("line_length : %u\n", ffsi->line_length);
	printf("=================================\n");
}

void plot_pixel(struct fb_config *fb, int x, int y, short color)
{
	int offset = (y * fb->width) + x;
	*((fb->data) + offset) = color;
}

void draw_rect(struct fb_config *fb, int x, int y, int w, int h, short color)
{
	int dx, dy;

	for (dy = 0; dy < h; dy++) {
		for (dx = 0; dx < w; dx++) {
			plot_pixel(fb, x + dx, y + dy, color);
		}
	}
}

void clear_screen(struct fb_config *fb, short color)
{
	draw_rect(fb, 0, 0, fb->width, fb->height, color);
}

void usage(const char *argv_0)
{
	printf("\nUsage %s: [-c<color>] [-B<border>] [-i<buffer index>]\n", argv_0);
	printf("  Color default to 0x0000\n");
	printf("  The border color applies to all rgb and is 10 pixels wide\n");
	printf("  If border is not provided, none is drawn.\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int fd;
	struct fb_var_screeninfo fvsi;
	struct fb_fix_screeninfo ffsi;
	struct fb_config fb;
	int border = -1;
	int opt;
	short color = 0;
	int cnt = 0;

	while ((opt = getopt(argc, argv, "c:B:i:h")) != -1) {
		switch (opt) {
		case 'c':
			color = strtol(optarg, NULL, 0);
			break;

		case 'B':
			border = 0x1F & strtol(optarg, NULL, 0);
			break;

		default:
			usage(argv[0]);
			break;
		}
	}

	memset(&fb, 0, sizeof(fb));

	if ((fd = open("/dev/fb0", O_RDWR)) < 0) {
		perror("open");
		exit(1);
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &fvsi) < 0) {
		perror("ioctl(FBIOGET_VSCREENINFO)");
		close(fd);
		exit(1);
	}

	dump_vscreeninfo(&fvsi);

	if (ioctl(fd, FBIOGET_FSCREENINFO, &ffsi) < 0) {
		perror("ioctl(FBIOGET_FSCREENINFO)");
		close(fd);
		exit(1);
	}

	dump_fscreeninfo(&ffsi);

	fb.fd = fd;
	fb.width = fvsi.xres;
	fb.height = fvsi.yres;
	fb.height_virtual = fvsi.yres_virtual;
	fb.bpp = fvsi.bits_per_pixel;
	fb.stride = ffsi.line_length;
	fb.red_offset = fvsi.red.offset;
	fb.red_length = fvsi.red.length;
	fb.green_offset = fvsi.green.offset;
	fb.green_length = fvsi.green.length;
	fb.blue_offset = fvsi.blue.offset;
	fb.blue_length = fvsi.blue.length;
	fb.transp_offset = fvsi.transp.offset;
	fb.transp_length = fvsi.transp.length;
	fb.buffer_num = fb.height_virtual / fb.height;
	
	short  buf_test[fb.width * fb.height];

	memset(buf_test, 0x00, sizeof(buf_test));
	fb.data = buf_test;

	while (1) {
		if (border == -1) {
			clear_screen(&fb, color);
			break;
		} else {
			clear_screen(&fb, 0x0000);

			draw_rect(&fb, border, border, fb.width - (border * 2 ),
				fb.height - (border * 2), rand()&0xFFFF);

			border += 5;
				if (border >= 50) border -= 50 ;
		}

	lseek(fd, 0, SEEK_SET);	
	cnt = write(fd, (char*)fb.data, 2*sizeof(buf_test));
		if (cnt <= 0) 
		{
			close(fd);
			return 1;
		}
	usleep(100000);
	}

	close(fd);

	return 0;
}

