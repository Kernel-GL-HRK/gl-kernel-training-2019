/*
 * fb_lcd.c
 * Author: Bekir Bekirov
 * Email: bekirbekirov1986@gmail.com
 */

#include "lcd_driver.h"

static struct fb_fix_screeninfo st7735sfb_fix = {
	.id     	= "ST7735S",
	.type       = FB_TYPE_PACKED_PIXELS,
	.visual     = FB_VISUAL_TRUECOLOR,
	.xpanstep   = 0,
	.ypanstep   = 0,
	.ywrapstep  = 0,
	.accel      = FB_ACCEL_NONE,
};


static struct fb_var_screeninfo st7735sfb_var = {
	.bits_per_pixel = 16,
};

static ssize_t st7735sfb_write(struct fb_info *info, const char __user *buf,
		size_t count, loff_t *ppos)
{
	unsigned long total_size;
	unsigned long p = *ppos;
	u8 __iomem *dst;

	total_size = info->fix.smem_len;

	if (p > total_size)
		return -EINVAL;

	if (count + p > total_size)
		count = total_size - p;

	if (!count)
		return -EINVAL;

	dst = (void __force *) (info->screen_base + p);

	if (copy_from_user(dst, buf, count))
		return -EFAULT;

	lcd_update_screen();

	*ppos += count;

	return count;
}

static int st7735sfb_blank(int blank_mode, struct fb_info *info)
{
	return 0;
}

static void st7735sfb_fillrect(struct fb_info *info,
			const struct fb_fillrect *rect)
{
	sys_fillrect(info, rect);
	lcd_update_screen();
}

static void st7735sfb_copyarea(struct fb_info *info,
			const struct fb_copyarea *area)
{
	sys_copyarea(info, area);
	lcd_update_screen();
}

static void st7735sfb_imageblit(struct fb_info *info,
			const struct fb_image *image)
{
	lcd_update_screen();
}

static int vfb_mmap(struct fb_info *info,
		    struct vm_area_struct *vma)
{
	return remap_vmalloc_range(vma, (void *)info->fix.smem_start,
							vma->vm_pgoff);
}

static struct fb_ops st7735sfb_ops = {
	.owner          = THIS_MODULE,
	.fb_read        = fb_sys_read,
	.fb_write       = st7735sfb_write,
	.fb_blank       = st7735sfb_blank,
	.fb_fillrect    = st7735sfb_fillrect,
	.fb_copyarea    = st7735sfb_copyarea,
	.fb_imageblit   = st7735sfb_imageblit,
	.fb_mmap		= vfb_mmap,
};

int init_fb_lcd(struct spi_device *spi)
{
	struct lcd_data *ptr_lcddata;
	int ret;
	struct fb_info *info;
	u32 vmem_size;
	u8 *vmem;

	ptr_lcddata = spi_get_drvdata(spi);
	info = framebuffer_alloc(0, &spi->dev);

		if (!info)
			return -ENOMEM;

	pr_info("frame buffer is allocated\n");
	ptr_lcddata->info = info;
	ptr_lcddata->width  = ST7735S_WIDTH;
	ptr_lcddata->height = ST7735S_HEIGHT;

	vmem = (u8 *)(ptr_lcddata->frame_buffer);
	vmem_size = ptr_lcddata->width * ptr_lcddata->height * 2;

	info->fbops = &st7735sfb_ops;
	info->fix = st7735sfb_fix;
	info->fix.line_length = ptr_lcddata->width * 2;

	info->var = st7735sfb_var;
	info->var.xres = ptr_lcddata->width;
	info->var.xres_virtual = ptr_lcddata->width;
	info->var.yres = ptr_lcddata->height;
	info->var.yres_virtual = ptr_lcddata->height;

	info->var.red.length = 5;
	info->var.red.offset = 11;
	info->var.green.length = 6;
	info->var.green.offset = 5;
	info->var.blue.length = 5;
	info->var.blue.offset = 0;

	info->screen_base = (u8 __force __iomem *)vmem;
	info->fix.smem_start = __pa(vmem);
	info->fix.smem_len = vmem_size;
	info->par = NULL;
	info->flags = FBINFO_FLAG_DEFAULT;

	fb_alloc_cmap(&info->cmap, 256, 0);
	ptr_lcddata->info = info;

	ret = register_framebuffer(info);
		if (ret) {
			pr_info("Error: ret = %d\n", ret);
			return ret;
		}
	pr_info("frame buffer is registered\n");
	return 0;
}

int deinit_fb_lcd(struct spi_device *spi)
{
	struct lcd_data *ptr_lcddata;

	ptr_lcddata = spi_get_drvdata(spi);
	unregister_framebuffer(ptr_lcddata->info);
	fb_dealloc_cmap(&ptr_lcddata->info->cmap);
	framebuffer_release(ptr_lcddata->info);

	if (!ptr_lcddata)
		return -ENODEV;

	pr_info("fb is unloaded!\n");

	return 0;
}
