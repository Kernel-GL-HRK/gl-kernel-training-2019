#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Dolinin");
MODULE_DESCRIPTION("Simple Linux module");
MODULE_VERSION("0.2");

#define DIR_NAME      "mymodule"
#define TO_UPPER_NAME "toupper"
#define TO_LOWER_NAME "tolower"
#define STAT_NAME     "stat"

static char to_upper_data[PAGE_SIZE];
static size_t to_upper_data_size = 0;
static size_t to_upper_calls = 0;
static size_t to_upper_chars_processed = 0;
static size_t to_upper_chars_converted = 0;

static char to_lower_data[PAGE_SIZE];
static size_t to_lower_data_size = 0;
static size_t to_lower_calls = 0;
static size_t to_lower_chars_processed = 0;
static size_t to_lower_chars_converted = 0;

void to_upper(const char *in_buf, char *out_buf, size_t count)
{
        size_t i;
        for (i = 0; i < count; ++i) {
                if (in_buf[i] >= 'a' && in_buf[i] <= 'z') {
                        out_buf[i] = in_buf[i] + 'A' - 'a';
                        ++to_upper_chars_converted;
                }
                ++to_upper_chars_processed;
        }
        
        ++to_upper_calls;
}

void to_lower(const char *in_buf, char *out_buf, size_t count)
{
        size_t i;
        for (i = 0; i < count; ++i) {
                if (in_buf[i] >= 'A' && in_buf[i] <= 'Z') {
                        out_buf[i] = in_buf[i] + 'a' - 'A';
                        ++to_lower_chars_converted;
                }
                ++to_lower_chars_processed;
        }
        
        ++to_lower_calls;
}

static ssize_t procfs_write(struct file *file, const char __user *pbuf,
        size_t count, loff_t *ppos)
{
        ssize_t not_copied = 0;
        
        if (count > PAGE_SIZE)
                return -ENOSPC;
        
        not_copied = copy_from_user(to_upper_data, pbuf, count);

        to_upper_data_size = count - not_copied;

        to_upper(to_upper_data, to_upper_data, to_upper_data_size);
        
        pr_info("mymodule: procfs_write %ld bytes\n", to_upper_data_size);

        return to_upper_data_size;
}

static ssize_t procfs_read(struct file *file, char __user *pbuf,
        size_t count, loff_t *ppos)
{
        static ssize_t attempts = 0;
        ssize_t not_copied = 0;
        ssize_t data_read = 0;

        if (attempts == 0) {
                data_read = min_t(ssize_t, to_upper_data_size, count);
                if(data_read) {
                        not_copied = copy_to_user(pbuf, to_upper_data,
                                data_read);
                        data_read -= not_copied;
                }

                pr_info("mymodule: procfs_read %ld bytes\n", data_read);
        }

        ++attempts;
        if (attempts == 3)
                attempts = 0;
        
        return data_read;
}

static ssize_t procfs_stat_read(struct file *file, char __user *pbuf,
        size_t count, loff_t *ppos)
{
        static ssize_t attempts = 0;
        ssize_t not_copied = 0;
        ssize_t data_read = 0;
        char stat_msg[200];
        ssize_t stat_msg_size;

        if (attempts == 0) {
                stat_msg_size = sprintf(stat_msg,
                        "Uppercase converter statistics:\n"
                        "\tTotal calls: %ld\n"
                        "\tCharacters processed: %ld\n"
                        "\tCharacters converted: %ld\n",
                        to_upper_calls, to_upper_chars_processed,
                        to_upper_chars_converted);

                data_read = min_t(ssize_t, stat_msg_size, count);
                if(data_read) {
                        not_copied = copy_to_user(pbuf, stat_msg, data_read);
                        data_read -= not_copied;
                }

                pr_info("mymodule: procfs_stat_read %ld bytes\n", data_read);
        }

        ++attempts;
        if (attempts == 3)
                attempts = 0;
        
        return data_read;
}

static ssize_t sysfs_show(struct class *class, struct class_attribute *attr,
        char *buf)
{
        
        pr_info("mymodule: sysfs_show %ld bytes\n", to_lower_data_size);

        sprintf(buf, "%s\n", to_lower_data);

        return to_lower_data_size;
}

static ssize_t sysfs_store(struct class *class, struct class_attribute *attr,
        const char *buf, size_t count)
{
        if (count > PAGE_SIZE)
                return -ENOSPC;
        
        sscanf(buf, "%s\n", to_lower_data);

        to_lower_data_size = strlen(to_lower_data);
        
        to_lower(to_lower_data, to_lower_data, to_lower_data_size);

        pr_info("mymodule: sysfs_store %ld bytes\n", to_lower_data_size);

        return count;
}

static ssize_t sysfs_stat_show(struct class *class,
        struct class_attribute *attr, char *buf)
{
        char stat_msg[200];
        ssize_t stat_msg_size;
        
        pr_info("mymodule: sysfs_stat_show %s\n", to_lower_data);

        stat_msg_size = sprintf(stat_msg,
                        "Lowercase converter statistics:\n"
                        "\tTotal calls: %ld\n"
                        "\tCharacters processed: %ld\n"
                        "\tCharacters converted: %ld\n",
                        to_lower_calls, to_lower_chars_processed,
                        to_lower_chars_converted);

        sprintf(buf, "%s\n", stat_msg);

        return stat_msg_size;
}

static struct file_operations to_upper_ops =
{
        .owner = THIS_MODULE,
        .write = procfs_write,
        .read = procfs_read,
};

static struct file_operations to_upper_stat_ops =
{
        .owner = THIS_MODULE,
        .read = procfs_stat_read,
};

static struct proc_dir_entry *dir;
static struct proc_dir_entry *to_upper_converter;
static struct proc_dir_entry *to_upper_stat;

struct class_attribute to_lower_class_attr_conv = {
        .attr = { .name = TO_LOWER_NAME, .mode = 0666 },
        .show = sysfs_show,
        .store = sysfs_store,
};

struct class_attribute to_lower_class_attr_stat = {
        .attr = { .name = STAT_NAME, .mode = 0444 },
        .show = sysfs_stat_show,
};

static struct class *to_lower_class;

static int mymodule_init(void)
{
        int ret;
        
        dir = proc_mkdir(DIR_NAME, NULL);
        if (!dir) {
                pr_err("mymodule: error creating procfs dirrectory "
                       "/proc/%s\n", DIR_NAME);
                return -ENOMEM;
        }

        to_upper_converter = proc_create(TO_UPPER_NAME, 0666, dir,
                &to_upper_ops);
        if (!to_upper_converter) {
                pr_err("mymodule: error creating procfs entry "
                       "/proc/mymodule/%s\n", TO_UPPER_NAME);
                remove_proc_entry(DIR_NAME, NULL);
                return -ENOMEM;
        }

        to_upper_stat = proc_create(STAT_NAME, 0444, dir, &to_upper_stat_ops);
        if (!to_upper_stat) {
                pr_err("mymodule: error creating procfs entry "
                       "/proc/mymodule/%s\n", STAT_NAME);
                remove_proc_entry(TO_UPPER_NAME, dir);
                remove_proc_entry(DIR_NAME, NULL);
                return -ENOMEM;
        }

        to_lower_class = class_create(THIS_MODULE, DIR_NAME);
        if (!to_lower_class) {
                pr_err("mymodule: error creating sysfs dirrectory "
                       "/sys/%s\n", DIR_NAME);
                return -ENOMEM;
        }

        ret = class_create_file(to_lower_class, &to_lower_class_attr_conv);
        if (ret) {
                pr_err("mymodule: error creating sysfs entry "
                       "/sys/%s\n", TO_LOWER_NAME);
                class_destroy(to_lower_class);
                return ret;
        }

        ret = class_create_file(to_lower_class, &to_lower_class_attr_stat);
        if (ret) {
                pr_err("mymodule: error creating sysfs entry " 
                      "/sys/%s\n", STAT_NAME);
                class_destroy(to_lower_class);
                return ret;
        }
		
        pr_info("mymodule: module loaded successfully\n");
        return 0;
}

static void mymodule_exit(void)
{
        proc_remove(dir);
        class_destroy(to_lower_class);
        pr_info("mymodule: module exited successfully\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);
