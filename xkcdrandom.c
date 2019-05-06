#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sebastian Morr");
MODULE_DESCRIPTION("A very good number generator.");
MODULE_VERSION("0.1.0");

#define DEVICE_NAME "xkcdrandom"
#define CLASS_NAME "xkcd"

static int major_number;
static struct class *class = NULL;
static struct device *device = NULL;

// Called each time the device is opened.
static int dev_open(struct inode *inodep, struct file *filep) { return 0; }

// Called whenever the device is being read from user space.
static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
                        loff_t *offset) {
    // Chosen by fair dice roll. Guaranteed to be random.
    char *message = "4";
    short size_of_message = 1;

    int error_count = copy_to_user(buffer, message, size_of_message);

    if (error_count == 0) {
        return size_of_message;
    } else {
        // Return a negative value, -14.
        return -EFAULT;
    }
}

// Called whenever the device is being written to. We just swallow everything
// thrown at us.
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
                         loff_t *offset) {
    return len;
}

// Called each time the device is closed. We don't need to do anything.
static int dev_release(struct inode *inodep, struct file *filep) { return 0; }

// Define our device operations in a struct, using C99's designated
// initializers.
static struct file_operations file_ops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

// Device node callback. I don't really understand when this is called.
// But we're using it to make the device world-redable.
static char *devnode(struct device *dev, umode_t *mode) {
    if (!mode) {
        return NULL;
    }
    if (MAJOR(dev->devt) == major_number) {
        *mode = 0666;
    }
    return NULL;
}

// Called when the module is loaded into the kernel.
static int __init xkcdrandom_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &file_ops);

    if (major_number < 0) {
        printk(KERN_ALERT "%s: Failed to register device.\n", DEVICE_NAME);
    } else {
        class = class_create(THIS_MODULE, CLASS_NAME);

        if (IS_ERR(class)) {
            unregister_chrdev(major_number, DEVICE_NAME);
            printk(KERN_ALERT "%s: Failed to register device class.\n",
                   DEVICE_NAME);
            return PTR_ERR(class);
        }

        class->devnode = devnode;

        device = device_create(class, NULL, MKDEV(major_number, 0), NULL,
                               DEVICE_NAME);

        if (IS_ERR(device)) {
            class_destroy(class);
            unregister_chrdev(major_number, DEVICE_NAME);
            printk(KERN_ALERT "%s: Failed to create device.\n", DEVICE_NAME);
            return PTR_ERR(device);
        }
    }

    return 0;
}

// Called when the module is removed from the kernel.
static void __exit xkcdrandom_cleanup(void) {
    device_destroy(class, MKDEV(major_number, 0));
    class_unregister(class);
    class_destroy(class);
    unregister_chrdev(major_number, DEVICE_NAME);
}

module_init(xkcdrandom_init);
module_exit(xkcdrandom_cleanup);
