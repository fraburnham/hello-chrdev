#include <asm-generic/fcntl.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>

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

struct file_operations hello_fops = {
  .owner = THIS_MODULE,
  .read = hello_read,
  .write = hello_write,
  .open = hello_open,
  .release = hello_release,
};

static int hello_init(void) {
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

  char *default_target = "world";
  memcpy(device->target, default_target, strlen(default_target));

  int error;
  error = cdev_add(&device->cdev, dev, 1);
  if (error) {
    printk(KERN_NOTICE "Error %d adding hello%d", error, dev);
  }

  return result;
}

static void hello_exit(void) {
  kfree(device);

  dev_t dev_num = MKDEV(hello_major, hello_minor);
  unregister_chrdev_region(dev_num, 1);

  return;
}

module_init(hello_init);
