#include "char_dev_module.h"

static int device_major_number; ///< Stores the device number

static struct device *eps_device = NULL; ///< pointer to the device driver struct
static struct class  *eps_class  = NULL; ///< pointer to the class struct that this 
                                         ///< device should be registered to

char tmp_buffer[BUFFER_LEN] = {0};
short n_bytes;

int char_dev_open(struct inode *pinode, struct file *pfile)
{
/** @Brief This function is called every time when the device is opened.
 * param[in] pinode that is a pointer to an inode object as defined in linux/fs.h
 * param[in] pfile that is a pointer to a file object as in linux/fs.h
 */
  printk(KERN_INFO "eps_char_dev: The device has been opened.\n"); 
  return 0;
}

ssize_t char_dev_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
/** @Brief Sends data from the device to the user space.
 *  Called whenever device is being read.
 *  param[in] pfile that is a pointer to a file object as in linux/fs.h
 *  param[in] buffer is a pointer to a buffer where data are written
 *  param[in] length represents the length of the buffer
 *  param[in] offset
 */
  int ret = -1;
  ///< Copy data from kernel space to user space.
  if(n_bytes)
  {
    ret = copy_to_user(buffer /* to */, 
                      tmp_buffer /* from */, 
                      n_bytes /* number of bytes to copy */);
  }
  else
  {
    printk(KERN_INFO "eps_char_dev: Nothing to send.\n");
    return 0;
  }
  
  if(!ret) ///< Succes.
  {
    printk(KERN_INFO "eps_char_dev: Successfully sent %d bytes.\n", n_bytes);
    memset(tmp_buffer, 0, BUFFER_LEN);
    n_bytes = 0;
    return 0;
  }
  else ///< Number of bytes that could not be copied.
  {
    printk(KERN_INFO "eps_char_dev: Failed to send %d bytes.\n", ret);
    return -EFAULT;
  }
  return 0;
}

ssize_t char_dev_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
/** @Brief Sends data from the user to the device.
 *  The received data is copied into tmp_buffer.
 *  param[in] pfile that is a pointer to a file object as in linux/fs.h
 *  param[in] buffer contains the string to write to the device
 *  param[in] length represents the length of the buffer
 *  param[in] offset
 */
  sprintf(tmp_buffer, "%s", buffer);
  n_bytes = strlen(tmp_buffer);
  printk(KERN_INFO "eps_char_dev: Received %d bytes\n", n_bytes);
  return length;
}

int char_dev_close(struct inode *pinode, struct file *pfile)
{
/** @Brief Releases the device.
 * param[in] pinode that is a pointer to an inode object as defined in linux/fs.h
 * param[in] pfile that is a pointer to a file object as in linux/fs.h
 */
  printk(KERN_INFO "eps_char_dev: The device has been closed.\n");
  return 0;
}

__init int char_dev_module_init(void)
{
/** @Brief The character device driver initialization function.
 *  Initializes the character device driver.
 */

  printk(KERN_ALERT "Initialization of char_dev_module.\n");
  ///< Register with the kernel
  device_major_number = register_chrdev(0 /* Major Number */, 
                                        DRIVER_NAME /* Name of the driver */,
                                        &char_dev_file_operations /* File Operations */);

  if(device_major_number < 0)
  {
    printk(KERN_ALERT "eps_char_dev: The driver failed to register with the kernel.\n");
  }
  else
  {
    printk(KERN_INFO "eps_char_dev: The driver has been successfully registered."
                     " dev_major_number %d.\n", device_major_number);
  }
  ///< Register the class of the device driver
  eps_class = class_create(THIS_MODULE, CLASS_NAME);

  if(IS_ERR(eps_class))
  {
    unregister_chrdev(device_major_number, DRIVER_NAME);
    printk(KERN_ALERT "eps_char_dev: Failed to register the class.\n");
    return PTR_ERR(eps_class);
  }
  else
  {
    printk(KERN_INFO "eps_char_dev: The class has been successfully registered.\n");
  }
  ///< Register the device driver
  eps_device = device_create(eps_class /* pointer to struct class */,
                             NULL /* parent */,
                             MKDEV(device_major_number, 0) /* the dev_t for the char \
                                                              device to be added */,
                             NULL /* driver data */,
                             DRIVER_NAME);

  if(IS_ERR(eps_device))
  {
    class_destroy(eps_class);
    unregister_chrdev(device_major_number, DRIVER_NAME);
    printk(KERN_ALERT "eps_char_dev: Failed to create the device.\n");
    return PTR_ERR(eps_device);
  }
  else
  {
    printk(KERN_INFO "eps_char_dev: The device has been successfully registered.\n");
  }
  return 0;
}

void char_dev_module_exit(void)
{
/** @Brief The character device driver release function.
 *  Unregisters the character device driver.
 */
  printk(KERN_ALERT "Removing eps_char_dev driver.\n");
  device_destroy(eps_class, MKDEV(device_major_number, 0));
  class_unregister(eps_class);
  class_destroy(eps_class);
  unregister_chrdev(device_major_number, DRIVER_NAME);
  printk(KERN_INFO "eps_char_dev: Successfully removed.\n");
}

module_init(char_dev_module_init);
module_exit(char_dev_module_exit);
