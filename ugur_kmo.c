#include <linux/init.h>	
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/wait.h>
#include <linux/poll.h>
//#include <linux/fs.h> 
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define DRIVER_AUTHOR	"Ugur ILTER <ugur.ilter@airties.com>"
#define DRIVER_DESC	"Character device module."
#define NAME		"char_device"

#define MAJOR_NUM 555
#define BUF_LEN 256

/* Prototypes */
static unsigned int device_poll(struct file *file, poll_table *wait);
static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset);
static ssize_t write_proc(struct file *file, const char __user * buffer, size_t length, loff_t * offset);
static int __init ugur_init(void);
static void __exit ugur_exit(void);

/* Global variables */
static char *msg_Ptr;
static char my_msg[BUF_LEN];
static wait_queue_head_t wait_queue;
struct proc_dir_entry *proc_dentry;
static int msg_len = 0;
static char *name;
module_param(name, charp, 0);


/* file operations */
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.poll = device_poll,
};

/* proc file operations */
static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.write = write_proc,
};

static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	int bytes_read = length > msg_len ? msg_len : length;

	printk("Device Read - %d\n", bytes_read);	

        if (msg_len == 0) return 0;

	/* msg from kspace -> uspace */
	copy_to_user(buffer, msg_Ptr++, bytes_read);
	msg_len -= bytes_read;

        return bytes_read;
}

static unsigned int device_poll(struct file *file, poll_table *wait)
{
	printk("Entering device poll\n");
	poll_wait(file, &wait_queue, wait);
	printk("After device poll\n");

	if (msg_len) return POLLIN | POLLRDNORM;

	return 0;
}

static ssize_t write_proc(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{
	printk("%s,%d buffer %d \n",__func__,__LINE__,length);
	copy_from_user(&my_msg, buffer, length);
	msg_len=length;
	msg_Ptr = my_msg;
	wake_up_interruptible(&wait_queue);
	return length;
}

void create_new_proc_entry(void)
{
	proc_dentry = proc_create("cdev_wait", 0666, NULL, &proc_fops);
   
	if(proc_dentry == NULL) {
		remove_proc_entry("cdev_wait", NULL);
        	return -ENOMEM;
	}
}



/* Called when device is loaded */
static int __init ugur_init(void)
{
	int ret;
	printk(KERN_INFO "Hello, %s!\n", name);
	ret = register_chrdev(MAJOR_NUM, NAME, &fops);
	init_waitqueue_head(&wait_queue);
	create_new_proc_entry();
	return ret;
}


/* Called on device cleanup */
static void __exit ugur_exit(void)
{
	printk(KERN_INFO "Goodbye, %s!\n", name);
	unregister_chrdev(MAJOR_NUM, NAME);
	remove_proc_entry("cdev_wait", NULL);
}

/* manually setting init & exit functions */
module_init(ugur_init);
module_exit(ugur_exit);

/* modinfo */
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
