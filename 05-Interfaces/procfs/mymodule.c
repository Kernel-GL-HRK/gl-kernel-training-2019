#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

#define DIR_NAME	"my_module"
#define CONV_FILE_NAME	"string-conv"
#define LOG_FILE_NAME	"log"

static struct proc_dir_entry *dir;
static struct proc_dir_entry *fstr, *flog;
static char msg[PAGE_SIZE];
static ssize_t msg_size;
static char bufout[PAGE_SIZE];
static ssize_t totalchar, totalwrite, totalread, totalcharconv, totaldigit;

static ssize_t sconv_read(struct file *file, char __user *pbuf, size_t count,
	loff_t *ppos)
{
	ssize_t num = 0, not_copied = 0;

	pr_info("mymodule-procfs: read file: %s\n",
		file->f_path.dentry->d_iname);
	pr_info("mymodule-procfs: read count bytes: %ld\n", count);

	if (*ppos > 0 || count < PAGE_SIZE)
		return 0;
	num = min_t(ssize_t, msg_size, count);
	not_copied = copy_to_user(pbuf, msg, num);
	if (not_copied)
		return -EFAULT;
	num -= not_copied;
	*ppos = num;
	totalread++;

	pr_info("mymodule-procfs: return bytes: %ld\n", num);

	return num;
}

static ssize_t sconv_write(struct file *file, const char __user *pbuf,
	size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	int i = 0;

	pr_info("mymodule-procfs: write file: %s\n",
		file->f_path.dentry->d_iname);
	pr_info("mymodule-procfs: write count bytes: %ld\n", count);

	if (*ppos > 0 || count > PAGE_SIZE)
		return -EFAULT;
	not_copied = copy_from_user(msg, pbuf, count);
	if (not_copied)
		return -EFAULT;
	msg[count] = '\0';
	msg_size = count - not_copied;
	*ppos = msg_size;
	totalwrite++;
	pr_info("mymodule-procfs: written bytes: %ld\n", msg_size);
	pr_info("mymodule-procfs: string: %s\n", msg);
	while (msg[i] != '\0') {
		if (isdigit(msg[i])) {
			totaldigit++;
		} else if (islower(msg[i])) {
			msg[i] = toupper(msg[i]);
			totalcharconv++;
		}
		i++;
	}
	totalchar += (i-1) ? i-1 : 0;
	pr_info("mymodule-procfs: to uppercase converter: %s\n", msg);

	return msg_size;
}

static ssize_t slog_read(struct file *file, char __user *pbuf, size_t count,
	loff_t *ppos)
{
	ssize_t num = 0, not_copied = 0;

	pr_info("mymodule-procfs: read file: %s\n",
		file->f_path.dentry->d_iname);
	pr_info("mymodule-procfs: read count bytes: %ld\n", count);

	if (*ppos > 0 || count < PAGE_SIZE)
		return 0;

	num = sprintf(bufout, "Total chars puts: %ld\n", totalchar);
	num += sprintf(bufout + num, "Total digits puts: %ld\n",
		totaldigit);
	num += sprintf(bufout + num,
		"Total chars to convertions: %ld\n", totalcharconv);
	num += sprintf(bufout + num,
		"Total number of write file \"%s\": %ld\n",
		CONV_FILE_NAME, totalwrite);
	num += sprintf(bufout + num,
		"Total number of read file \"%s\": %ld\n",
		CONV_FILE_NAME, totalread);

	not_copied = copy_to_user(pbuf, bufout, num);
	if (not_copied)
		return -EFAULT;
	num -= not_copied;
	*ppos = num;

	pr_info("mymodule-procfs: return bytes: %ld\n", num);

	return num;
}

static const struct file_operations sconv_ops = {
	.owner = THIS_MODULE,
	.read = sconv_read,
	.write = sconv_write,
};

static const struct file_operations slog_ops = {
	.owner = THIS_MODULE,
	.read = slog_read,
};

static int __init init_mod(void)
{
	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("mymodule-procfs: error creating procfs directory: %s\n",
			DIR_NAME);
		return -ENOMEM;
	}

	fstr = proc_create(CONV_FILE_NAME, 0666, dir, &sconv_ops);
	if (fstr == NULL) {
		pr_err("mymodule-procfs: error creating procfs file: %s\n",
			CONV_FILE_NAME);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	flog = proc_create(LOG_FILE_NAME, 0444, dir, &slog_ops);
	if (flog == NULL) {
		pr_err("mymodule-procfs: error creating procfs file: %s\n",
			LOG_FILE_NAME);
		remove_proc_entry(CONV_FILE_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("mymodule-procfs: module loaded\n");
	return 0;
}

static void __exit exit_mod(void)
{
	proc_remove(dir);
	pr_info("mymodule-procfs: module removed\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Domnin");
MODULE_DESCRIPTION("A simple procfs module");
MODULE_VERSION("0.1");
