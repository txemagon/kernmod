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
static ssize_t device_write   (struct file *,  const char *, size_t, loff_t *);


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
    .open  = device_open,
    .release = device_release
};

int init_module(void) {
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk( KERN_ALERT "Registering char device failed with %d.\n", Major);
        return Major;
    }

    printk( KERN_INFO "I was assigned major number %d. To talk to\n", Major );
    printk( KERN_INFO "the driver, create a dev file with\n" );
    printk( KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major );
    printk( KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk( KERN_INFO "the device file.\n");
    printk( KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

void cleanup_module(void) {
    /*
    int ret = unregister_chrdev(Major, DEVICE_NAME);
    if (ret < 0)
        printk( KERN_ALERT "Error in unregister_chrdev: %d.\n", ret );
     */
    unregister_chrdev(Major, DEVICE_NAME);
}

/* METHODS */
/* Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file) {
    static int counter = 0;

    if (Device_open)
        return -EBUSY;

    Device_open++;
    sprintf(msg, "I already told you %d times Hello World!\n", counter++);
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file) {
    Device_open--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read( struct file *filp,         /* linux/fs.h */
                            char *buffer,              /* buffer to fill with data */
                            size_t length,             /* length of the buffer */
                            loff_t *offset) {

    /* Number of bytes actually written to buffer */
    int bytes_read = 0;

    /* return 0 at end of mssg, signifyig EOF */
    if (*msg_Ptr == 0)
        return 0;

    /* Put data into buffer */
    while (length && *msg_Ptr){
        /* copy from kernel space to user space */
        put_user(*(msg_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
    return bytes_read;
}

/* called on: echo "Hi" > /dev/hello */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *loff) {
    printk( KERN_ALERT "Sorry, this operation isn't supported.\n" );
    return -EINVAL;
}
