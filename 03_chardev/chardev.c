#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>  /* For put_user */

/* Prototypes (normally in a header file */
int init_module(void);
void cleanup_module(void);
static int     device_open    (struct inode *, struct file*);
static int     device_release (struct inode *, struct file*);
static ssize_t device_read    (struct file *,  char *, size_t, loff_t *);
static ssize_t device_write   (struct file *,  char *, size_t, loff_t *);


#define SUCCESS 0
#define DEVICE_NAME "chardev" /* Device name for /proc/devices */
#define BUF_LEN     80        /* Max length of mssg from the device */

static int Major;             /* Assigned to our device driver */
static int Device_open = 0;   /* Is our device open? */

static char msg[BUF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
    .read  = device_read,
    .write = device_write,
    .open  = Device_open,
    .release = device_release
};

