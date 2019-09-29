#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h> ///< MKDEV, macros
#include <linux/uaccess.h> ///< copy_to_user
#include <linux/fs.h> ///< Used for character driver support

#define DRIVER_NAME "eps_char_dev"
#define CLASS_NAME  "epsilon"

#define BUFFER_LEN  1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandru - George Rusu");
MODULE_DESCRIPTION("This my first kernel driver");
MODULE_VERSION("1.0");

int char_dev_open(struct inode *pinode, struct file *pfile);
ssize_t char_dev_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t char_dev_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
int char_dev_close(struct inode *pinode, struct file *pfile);

/** @brief structure from linux/fs.h that lists the callback functions 
 *  performed on this device.
 */
struct file_operations char_dev_file_operations = {
	.owner   = THIS_MODULE,
	.open    = char_dev_open,
	.read    = char_dev_read,
	.write   = char_dev_write,
	.release = char_dev_close,
};
