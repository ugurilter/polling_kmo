#include <linux/init.h>	
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/printk.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <uapi/linux/stat.h>

#define DRIVER_AUTHOR	"Ugur ILTER <ugur.ilter@airties.com>"
#define DRIVER_DESC	"Character device module."
#define NAME		"char_device"

#define MAJOR_NUM 555
#define BUF_LEN 256

static char *msg_Ptr;
static char my_msg[BUF_LEN];
static int len;

static wait_queue_head_t wait_queue;

static char* name;
module_param(name, charp, 0);

static unsigned int device_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &wait_queue, wait);

	if (len > 0)
		return POLLIN | POLLRDNORM;
	else
		return POLLHUP | POLLWRNORM;

	return 0;
}

static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	int bytes_read = 0;

        if (*msg_Ptr == 0) return 0;

	/* msg from kspace -> uspace */
        while (length && *msg_Ptr) {
                put_user(*(msg_Ptr++), buffer++); // (val, dest addr)
                length--;
		len--;
                bytes_read++;
        }

        return bytes_read;

}

static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{
	int i;

	/* Clearing buffer */
	for(i = 0; i < BUF_LEN; i++) my_msg[i] = 0;

	/* msg from uspace -> kspace */
	for (i = 0; i < length && i < BUF_LEN; i++){
		get_user(my_msg[i], buffer + i); // (var, src addr)
		len++;
	}

	msg_Ptr = my_msg;

	return i;
}

/* file operations */
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.poll = device_poll,
};

/* Called when device is loaded */
static int __init ugur_init(void)
{
	int ret;
	printk(KERN_INFO "Hello, %s!\n", name);
	ret = register_chrdev(MAJOR_NUM, NAME, &fops);
	init_waitqueue_head(&wait_queue);
	return ret;
}


/* Called on device cleanup */
static void __exit ugur_exit(void)
{
	printk(KERN_INFO "Goodbye, %s!\n", name);
	unregister_chrdev(MAJOR_NUM, NAME);
}

/* manually setting init & exit functions */
module_init(ugur_init);
module_exit(ugur_exit);

/* modinfo */
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);