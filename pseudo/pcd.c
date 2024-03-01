#include<linux/module.h>			
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#define DEV_MEM_SIZE  512

#undef pr_fmt 
#define pr_fmt(fmt) "%s:" fmt, __func__	


/*pseudo device's memory*/
char device_buffer[DEV_MEM_SIZE];

/*that holds the device number*/
dev_t device_number;

/*c_dev varible*/
struct cdev pcd_cdev;	

loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
 	loff_t temp;
	
	pr_info("lseek requested");
	pr_info("current file pos = %lld\n",filp->f_pos);

	switch(whence)
	{	
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0))
				return -EINVAL;
			filp->f_pos = offset;
			break; 
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
		  		return -EINVAL;
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
                        if((temp > DEV_MEM_SIZE) || (temp < 0))
                                return -EINVAL;
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
	pr_info(" new file pos = %lld\n",filp->f_pos);
	return filp->f_pos;
}
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("read requested for %zu bytes \n",count);
	pr_info("current pos: %lld",*f_pos);	
	
	/*adjust the count*/
      	if(*f_pos + count>DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;

	/*copy to user*/
	if(copy_to_user(buff,&device_buffer[*f_pos],count)){
		return -EFAULT;
	}

	/*update the pos*/
	*f_pos += count;
	
	pr_info("number of bytes successfully read = %zu\n", count);
	pr_info("updated file pos: %lld", *f_pos);
	
	/*return the number of bytes*/
	return 0;
	}
	ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
	{
	pr_info("write requested for %zu bytes \n",count);
        pr_info("current pos: %lld",*f_pos);
        /*adjust the count*/
        if(*f_pos + count>DEV_MEM_SIZE)
                count = DEV_MEM_SIZE - *f_pos;
	if(!count)
		return -ENOMEM;
        /*copy from user*/
        if(copy_from_user(&device_buffer[*f_pos],buff,count)){
                return -EFAULT;
        }

        /*update the pos*/
        *f_pos += count;

        pr_info("number of bytes successfully written = %zu\n", count);
        pr_info("updated file pos: %lld", *f_pos);
	
	
	return 0;
}
int pcd_open (struct inode *inode, struct file *filp)
{
	pr_info("Open is successfull\n");
	return 0;
}
int pcd_release (struct inode *inode, struct file *flip)
{
	pr_info("release/close is successfull\n");
	return 0;
}

struct file_operations pcd_fops =
{
	.open = pcd_open,
	.write = pcd_write,	
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
    	.owner = THIS_MODULE
};

struct device *device_pcd;
struct class *class_pcd;

static int __init pcd_driver_init(void)
{
	/*1.dynamically allocate the device number*/
        alloc_chrdev_region(&device_number,0,1,"pcd_devices");
	pr_info("Device_number <MAJOR><MINOR> %d:%d\n",MAJOR(device_number),MINOR(device_number));
        
	/*2. initialize the cdev structure with fops*/
	pcd_cdev.owner = THIS_MODULE;
	cdev_init(&pcd_cdev, &pcd_fops);
	cdev_add(&pcd_cdev,device_number,1);	
	
	/*create device class under /sys/class/ */
	class_pcd = class_create("pcd_class");
	
	/*populate the sysfs with device information */
	device_pcd = device_create(class_pcd,NULL,device_number,NULL,"pcd");	
	pr_info("Module init is successfull\n");

	return 0;

}

static void __exit pcd_driver_cleanup(void)
{
	device_destroy(class_pcd,device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number, 1);
	pr_info("Module unloaded");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("peeyush");
MODULE_DESCRIPTION("A pseudo character driver");
