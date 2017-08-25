#include <linux/init.h>	
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/wait.h>
#include <linux/poll.h>

#define DRIVER_AUTHOR	"Ugur ILTER <ugur.ilter@airties.com>"
#define DRIVER_DESC	"Character device module."
#define NAME		"char_device"

#define MAJOR_NUM 555
#define BUF_LEN 256

/* Prototypes */
static unsigned int device_poll(struct file *file, poll_table *wait);
static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset);
static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset);
static int __init ugur_init(void);
static void __exit ugur_exit(void);

/* Global variables */
static char *msg_Ptr;
static char my_msg[BUF_LEN];
static int flag;
static wait_queue_head_t wait_queue;

static char *name;
module_param(name, charp, 0);


/* file operations */
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.poll = device_poll,
};


static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	int bytes_read = length;

        if (*msg_Ptr == 0) return 0;

	/* msg from kspace -> uspace */
	copy_to_user(buffer, msg_Ptr, length);

	/* clear flag */
	flag = 0;

        return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{
	/* msg from uspace -> kspace */
	copy_from_user(&my_msg, buffer, length);

	/* set flag */
	flag = 1;

	msg_Ptr = my_msg;

	wake_up_interruptible(&wait_queue);

	return length;
}

static unsigned int device_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &wait_queue, wait);

	if (flag) return POLLIN | POLLRDNORM;

	return 0;
}

/* Called when device is loaded */
static int __init ugur_init(void)
{
	int ret;
	printk(KERN_INFO "Hello, %s!\n", name);
	ret = register_chrdev(MAJOR_NUM, NAME, &fops);
	init_waitqueue_head(&wait_queue);
	//proc_create(NAME, 0, NULL, &fops);
	return ret;
}


/* Called on device cleanup */
static void __exit ugur_exit(void)
{
	printk(KERN_INFO "Goodbye, %s!\n", name);
	unregister_chrdev(MAJOR_NUM, NAME);
	//remove_proc_entry(NAME, NULL);
}

/* manually setting init & exit functions */
module_init(ugur_init);
module_exit(ugur_exit);

/* modinfo */
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
