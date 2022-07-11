// this file is generated using org and babel from hello-chrdev.org
#include <asm-generic/fcntl.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>

int hello_major = 0;
int hello_minor = 0;

#ifndef TARGET_MAX_LEN
#define TARGET_MAX_LEN 64
#endif

struct hello_dev {
  char target[TARGET_MAX_LEN + 1];
  struct cdev cdev;
};

struct hello_dev *device;

int hello_open(struct inode *i, struct file *f) {
  if ((f->f_flags & O_ACCMODE) == O_WRONLY) {
    memset(device->target, 0, TARGET_MAX_LEN + 1);
  }

  return 0;
}

int hello_release(struct inode *i, struct file *f) {
  return 0;
}

ssize_t hello_read(struct file *f, char __user *buf, size_t read_amount, loff_t *offset) {
  long leftover, transferred, remaining_bytes;
  char data[72] = "Hello ";

  memcpy(data + strlen(data), device->target, strlen(device->target));

  remaining_bytes = strlen(data) - *offset;
  if (remaining_bytes <= 0) {
    transferred = 0;
  } else if (read_amount > remaining_bytes) {
    leftover = copy_to_user(buf, data + *offset, remaining_bytes);
    transferred = remaining_bytes - leftover;
  } else {
    leftover = copy_to_user(buf, data + *offset, read_amount);
    transferred = read_amount - leftover;
  }

  *offset = *offset + transferred;
  
  return transferred;
}

ssize_t hello_write(struct file *f, const char __user *buf, size_t write_amount, loff_t *offset) {
  long leftover, transferred, remaining_bytes;
  remaining_bytes = TARGET_MAX_LEN - *offset;
  if (remaining_bytes <= 0) {
    transferred = 0;
  } else if (write_amount > remaining_bytes) {
    leftover = copy_from_user(device->target + *offset, buf, remaining_bytes);
    transferred = write_amount - leftover;
  } else {
    leftover = copy_from_user(device->target + *offset, buf, write_amount);
    transferred = write_amount - leftover;
  }

  *offset = *offset + transferred;
  return transferred;
} 

struct file_operations hello_fops = {
  .owner = THIS_MODULE,
  .read = hello_read,
  .write = hello_write,
  .open = hello_open,
  .release = hello_release,
};

static int hello_init(void) {
  int error;
  char *default_target = "world";
  dev_t dev;
  int result = 0;
  
  result = alloc_chrdev_region(&dev, hello_minor, 1, "hello-dev");
  hello_major = MAJOR(dev);
  
  if (result < 0) {
    printk(KERN_WARNING "hello: can't get major %d", hello_major);
    return result;
  }

  device = kmalloc(sizeof(struct hello_dev), GFP_KERNEL);
  if (!device) {
    result = -ENOMEM;
   }
  memset(device, 0, sizeof(struct hello_dev));

  cdev_init(&device->cdev, &hello_fops);
  device->cdev.owner = THIS_MODULE;
  device->cdev.ops = &hello_fops;

  memcpy(device->target, default_target, strlen(default_target));

  error = cdev_add(&device->cdev, dev, 1);
  if (error) {
    printk(KERN_NOTICE "Error %d adding hello%d", error, dev);
  }

  return result;
}

static void hello_exit(void) {
  dev_t dev_num = MKDEV(hello_major, hello_minor);

  kfree(device);

  unregister_chrdev_region(dev_num, 1);

  return;
}

MODULE_LICENSE("Dual BSD/GPL");
module_init(hello_init);
module_exit(hello_exit);
