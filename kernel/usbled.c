/*
 * Usb Skeleton driver - 2.2
 *
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 * This driver is based on the 2.6.3 version of drivers/usb/usb-skeleton.c
 * but has been rewritten to be easier to read and use.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>


#define USBLED_CMD_ON 1
#define USBLED_CMD_OFF 0

#define SET_CONTROL_REQUEST_TYPE        0x21
#define SET_CONTROL_REQUEST             0x22


/* Define these values to match your devices */
#define USB_USBLED_VENDOR_ID	0x16c0
#define USB_USBLED_PRODUCT_ID	0x05dc

/* table of devices that work with this driver */
static struct usb_device_id usbled_table [] = {
  { USB_DEVICE(USB_USBLED_VENDOR_ID, USB_USBLED_PRODUCT_ID) },
  { }					/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, usbled_table);


/* Get a minor range for your devices from the usb maintainer */
#define USB_USBLED_MINOR_BASE	192

/* our private defines. if this grows any larger, use your own .h file */
#define MAX_TRANSFER		(PAGE_SIZE - 512)
/* MAX_TRANSFER is chosen so that the VM is not stressed by
   allocations > PAGE_SIZE and the number of packets in a page
   is an integer 512 is the largest possible packet on EHCI */
#define WRITES_IN_FLIGHT	8
/* arbitrarily chosen */

/* Structure to hold all of our device specific stuff */
struct usb_usbled {
  struct usb_device	*udev;			/* the usb device for this device */
  struct usb_interface	*interface;		/* the interface for this device */
  struct semaphore	limit_sem;		/* limiting the number of writes in progress */
  struct usb_anchor	submitted;		/* in case we need to retract our submissions */

  int			errors;			/* the last request tanked */
  int			open_count;		/* count the number of openers */
  spinlock_t		err_lock;		/* lock for errors */
  struct kref		kref;
  struct mutex		io_mutex;		/* synchronize I/O with disconnect */
};
#define to_usbled_dev(d) container_of(d, struct usb_usbled, kref)

static struct usb_driver usbled_driver;

static void usbled_delete(struct kref *kref)
{
  struct usb_usbled *dev = to_usbled_dev(kref);

  usb_put_dev(dev->udev);
  kfree(dev);
}

static int usbled_open(struct inode *inode, struct file *file)
{
  struct usb_usbled *dev;
  struct usb_interface *interface;
  int subminor;
  int retval = 0;

  subminor = iminor(inode);

  interface = usb_find_interface(&usbled_driver, subminor);
  if (!interface) {
    err ("%s - error, can't find device for minor %d",
	 __FUNCTION__, subminor);
    retval = -ENODEV;
    goto exit;
  }

  dev = usb_get_intfdata(interface);
  if (!dev) {
    retval = -ENODEV;
    goto exit;
  }

  /* increment our usage count for the device */
  kref_get(&dev->kref);

  /* lock the device to allow correctly handling errors
   * in resumption */
  //  mutex_lock(&dev->io_mutex);

/*   if (!dev->open_count++) { */
/*     retval = usb_autopm_get_interface(interface); */
/*     if (retval) { */
/*       dev->open_count--; */
/*       mutex_unlock(&dev->io_mutex); */
/*       kref_put(&dev->kref, usbled_delete); */
/*       goto exit; */
/*     } */
/*   } */ /* else { //uncomment this block if you want exclusive open
       retval = -EBUSY;
       dev->open_count--;
       mutex_unlock(&dev->io_mutex);
       kref_put(&dev->kref, usbled_delete);
       goto exit;
       } */
  /* prevent the device from being autosuspended */

  /* save our object in the file's private structure */
  file->private_data = dev;

 exit:
  return retval;
}

static int usbled_release(struct inode *inode, struct file *file)
{
  struct usb_usbled *dev;

  dev = (struct usb_usbled *)file->private_data;
  if (dev == NULL)
    return -ENODEV;

  /* allow the device to be autosuspended */
  mutex_lock(&dev->io_mutex);
  if (!--dev->open_count && dev->interface)
    usb_autopm_put_interface(dev->interface);
  mutex_unlock(&dev->io_mutex);

  /* decrement the count on our device */
  kref_put(&dev->kref, usbled_delete);
  return 0;
}

static int usbled_flush(struct file *file, fl_owner_t id)
{
  struct usb_usbled *dev;
  int res;

  dev = (struct usb_usbled *)file->private_data;
  if (dev == NULL)
    return -ENODEV;

  /* wait for io to stop */
  mutex_lock(&dev->io_mutex);

  /* read out errors, leave subsequent opens a clean slate */
  spin_lock_irq(&dev->err_lock);
  res = dev->errors ? (dev->errors == -EPIPE ? -EPIPE : -EIO) : 0;
  dev->errors = 0;
  spin_unlock_irq(&dev->err_lock);

  mutex_unlock(&dev->io_mutex);

  return res;
}

static ssize_t usbled_write(struct file *file, 
			    const char *user_buffer, 
			    size_t count, 
			    loff_t *ppos)
{
  struct usb_usbled *dev;
  int retval = 0;
  char *buf = NULL;
  size_t writesize = min(count, (size_t)MAX_TRANSFER);
  __u16 value = 0;

  dev = (struct usb_usbled *)file->private_data;

  /* verify that we actually have some data to write */
  if (count == 0)
    goto exit;

  /* limit the number of URBs in flight to stop a user from using up all RAM */
  if (down_interruptible(&dev->limit_sem)) {
    retval = -ERESTARTSYS;
    goto exit;
  }

  spin_lock_irq(&dev->err_lock);
  if ((retval = dev->errors) < 0) {
    /* any error is reported once */
    dev->errors = 0;
    /* to preserve notifications about reset */
    retval = (retval == -EPIPE) ? retval : -EIO;
  }
  spin_unlock_irq(&dev->err_lock);
  if (retval < 0)
    goto error;

  buf = kmalloc(writesize, GFP_KERNEL);
  if (!buf) {
    retval = -ENOMEM;
    goto error;
  }

  if (copy_from_user(buf, user_buffer, writesize)) {
    retval = -EFAULT;
    goto error;
  }

  /* this lock makes sure we don't submit URBs to gone devices */
  mutex_lock(&dev->io_mutex);
  if (!dev->interface) {		/* disconnect() was called */
    mutex_unlock(&dev->io_mutex);
    retval = -ENODEV;
    goto error;
  }


  if ( strncmp(buf, "1", 1) == 0 ) {
    value = USBLED_CMD_ON;
  }
  else {
    value = USBLED_CMD_OFF;
  }
    
  retval = usb_control_msg(dev->udev,
			   usb_sndctrlpipe(dev->udev, 0),
			   value,
			   244, // magic REQUEST_TYPE
			   value,
			   value,
			   NULL,
			   0,
			   5000);

  mutex_unlock(&dev->io_mutex);

  return writesize;

 error:
  kfree(buf);
  up(&dev->limit_sem);

 exit:
  return retval;
}

static const struct file_operations usbled_fops = {
  .owner =	THIS_MODULE,
  .write =	usbled_write,
  .open =	usbled_open,
  .release =	usbled_release,
  .flush =	usbled_flush,
};

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with the driver core
 */
static struct usb_class_driver usbled_class = {
  .name =		"usbled%d",
  .fops =		&usbled_fops,
  .minor_base =	USB_USBLED_MINOR_BASE,
};

static int usbled_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  struct usb_usbled *dev;
  struct usb_host_interface *iface_desc;
  int retval = -ENOMEM;

  /* allocate memory for our device state and initialize it */
  dev = kzalloc(sizeof(*dev), GFP_KERNEL);
  if (!dev) {
    err("Out of memory\n");
    goto error;
  }
  kref_init(&dev->kref);
  sema_init(&dev->limit_sem, WRITES_IN_FLIGHT);
  mutex_init(&dev->io_mutex);
  spin_lock_init(&dev->err_lock);

  dev->udev = usb_get_dev(interface_to_usbdev(interface));
  dev->interface = interface;

  /* set up the endpoint information */
  /* use only the first bulk-in and bulk-out endpoints */
  iface_desc = interface->cur_altsetting;
	
  /* save our data pointer in this interface device */
  usb_set_intfdata(interface, dev);

  /* we can register the device now, as it is ready */
  retval = usb_register_dev(interface, &usbled_class);
  if (retval) {
    /* something prevented us from registering this driver */
    err("Not able to get a minor for this device.\n");
    usb_set_intfdata(interface, NULL);
    goto error;
  }

  /* let the user know what node this device is now attached to */
  info("USB Usbled device now attached to usbled-%d", interface->minor);
  return 0;

 error:
  if (dev)
    /* this frees allocated memory */
    kref_put(&dev->kref, usbled_delete);
  return retval;
}

static void usbled_disconnect(struct usb_interface *interface)
{
  struct usb_usbled *dev;
  int minor = interface->minor;

  dev = usb_get_intfdata(interface);
  usb_set_intfdata(interface, NULL);

  /* give back our minor */
  usb_deregister_dev(interface, &usbled_class);

  /* prevent more I/O from starting */
  mutex_lock(&dev->io_mutex);
  dev->interface = NULL;
  mutex_unlock(&dev->io_mutex);


  /* decrement our usage count */
  kref_put(&dev->kref, usbled_delete);

  info("usbled #%d now disconnected", minor);
}

static struct usb_driver usbled_driver = {
  .name =		"usbled",
  .probe =	usbled_probe,
  .disconnect =	usbled_disconnect,
  .id_table =	usbled_table,
  .supports_autosuspend = 1,
};

static int __init usb_usbled_init(void)
{
  int result;

  /* register this driver with the USB subsystem */
  result = usb_register(&usbled_driver);
  if (result)
    err("usb_register failed. Error number %d", result);

  return result;
}

static void __exit usb_usbled_exit(void)
{
  /* deregister this driver with the USB subsystem */
  usb_deregister(&usbled_driver);
}

module_init(usb_usbled_init);
module_exit(usb_usbled_exit);

MODULE_LICENSE("GPL");
