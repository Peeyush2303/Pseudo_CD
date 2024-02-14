#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#define Dev_mem_size  512

char device_buffer[Dev_mem_size];

dev_t device_number;

struct cdev pcd_cdev;

loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
{
return 0;
}
ssize_t pcd_read(struct file *filp, char user *buff, size_t count, loff_t *f_pos)
{
return 0;
}
ssize_t pcd_write(struct file *filp, const char user *buff, size_t count, loff_t *f_pos)
{
return 0;
}
int pcd_open (struct inode *inode, struct file *filp)
{
return 0;
}
int pcd_release (struct inode *inode, struct file *flip)
{
return 0;
}

struct file_operations pcd_fops;
{

	.open = pcd_open,
	.write = pcd_write,	
	.read = pcd_read,
	.lseek = pcd_lseek,
	.release = pcd_release,
    	.owner = THIS_MODULE
};

struct device *device_pcd;

static int __init pcd_driver_init(void)
{
        alloc_chrdev_region(&device_number,0,1,"pcd_devices");
	
	pr_info("%s: Device_number <MAJOR><MINOR> %d:%d\n",__func__,MAJOR(device_number),MINOR(device_number));

	cdev_init(&pcd_cdev, &pcd_fops);
	pcd_cdev.owner = THIS_MODULE;
	cdev_add(&pcd_cdev,device_number,1);	
/*create device class under /sys/class/ */
	class_pcd = class_create(THIS_MODULE,"pcd_class");
/*populate the sysfs with device information */

	device_pcd = device_create(class_pcd,NULL,device_number,NULL);	
	
	pr_info("Module init is successfull\n");

	return 0;
	
}

static void __exit pcd_driver_cleanup(void)

{

}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("peeyush");
MODULE_DESCRIPTION("A pseudo character driver");
