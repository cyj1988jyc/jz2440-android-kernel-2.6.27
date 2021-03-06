/*
	aaron.yj
*/

#include <linux/device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>
#include <linux/cdev.h>

volatile unsigned long* gpfcon = NULL;
volatile unsigned long* gpfdat = NULL;
static struct class * led_dev_class;
static struct cdev first_dev;
static dev_t n_dev;
#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (0 << (2 * 0)) // in
int led_open (struct inode * inode, struct file * file)
{
    *gpfcon &= ~(GPF4_MASK | GPF0_MASK); //
    *gpfcon |= (GPF4_CON | GPF0_CON); //

	return 0;
}

ssize_t led_write (struct file * file, const char __user * buffer, size_t count, loff_t * fpos)
{	
	printk("33333");
	int val;
	copy_from_user(&val,buffer,count);
	if (val == 1)
		*gpfdat &= ~(1 << 4);
	else
                *gpfdat |= (1 << 4);
	printk("444444");

	return count;
}

static struct file_operations led_operations = {
   .owner = THIS_MODULE,
   .open = led_open,
   .write = led_write,
};

int major;
dev_t devt;
static int __init jz2440_led_init()
{
	int ret;
	printk("11111\n");
	ret = alloc_chrdev_region(&n_dev, 0, 1, "first_dev");
	if (ret < 0)
		return ret;
	cdev_init(&first_dev, &led_operations);
	ret = cdev_add(&first_dev, n_dev, 1);
	if (ret < 0)
		return ret;
	led_dev_class = class_create(THIS_MODULE,"first_dev");
	device_create(led_dev_class,NULL,n_dev,NULL,"xyz");
	gpfcon = (volatile unsigned long*)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;
    printk("22222\n");

	return 0;
}
static void __exit jz2440_led_exit()
{
	cdev_del(&first_dev);
	unregister_chrdev_region(n_dev, 1);
	class_destroy(led_dev_class);
	device_destroy(led_dev_class,devt);
	iounmap(gpfcon);
}


module_init(jz2440_led_init);
module_exit(jz2440_led_exit);
MODULE_LICENSE("GPL");
