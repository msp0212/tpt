#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#include "driver_ioctl.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int code = 20020;
static int val = 10010;

static int driver_open(struct inode *i, struct file *f)
{
	return 0;
}

static int driver_close(struct inode *i, struct file *f)
{
	return 0;
}

static long driver_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	test_ioctl_t io;

	switch(cmd) {
	case TEST_GET:
		io.code = code;
		io.val = val;
		if (copy_to_user((test_ioctl_t *)arg, &io, sizeof(test_ioctl_t))) {
			return -EACCES;
		}
		break;
	case TEST_SET:
		if (copy_from_user(&io, (test_ioctl_t *)arg, sizeof(test_ioctl_t))) {
			return -EACCES;
		}
		code = io.code;
		val = io.val;
	}
	return 0;
}

static struct file_operations driver_fo = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.unlocked_ioctl = driver_ioctl
};

static int __init driver_ioctl_init(void)
{
	int ret;
	struct device *dev_ret;


	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "driver_ioctl")) < 0)
	{
		return ret;
	}

	cdev_init(&c_dev, &driver_fo);

	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
	{
		return ret;
	}

	if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
	{
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "driver")))
	{
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}

	return 0;
}

static void __exit driver_ioctl_exit(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(driver_ioctl_init);
module_exit(driver_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohit Singh");
MODULE_DESCRIPTION("Driver IOCTL TEST");
