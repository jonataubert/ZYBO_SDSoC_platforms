//Kernel module which allocates 3 frame buffers for VDMA
//Frame buffer size is 1920*1080*2 Byte
//Author : Jonathan Aubert
//Date : 24 June 2016
//License : GNU GENERAL PUBLIC LICENSE Version 2, June 1991
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

//my 3 buffers
void *my_kernel_buffer0,  *my_kernel_buffer1,  *my_kernel_buffer2;

struct my_sysfs_attributes {
	uint32_t buffer_base_phys0;	// RD only generic
	uint32_t buffer_base_phys1;
	uint32_t buffer_base_phys2;
};

struct my_sysfs_attributes the_my_sysfs_attributes = {
	.buffer_base_phys0	= 0,
	.buffer_base_phys1	= 0,
	.buffer_base_phys2	= 0,
};

//1920*1080*2 = 4147200 bytes -> 1013 PAGE of 4096Byte
#define BUFFER_SIZE 1013*PAGE_SIZE

//when read attribute (file)
#define MY_DRIVER_ATTR_RO(name) \
ssize_t name##_show(struct device_driver *driver, char *buf) \
{ \
	return scnprintf(buf, BUFFER_SIZE, "0x%08X\n", the_my_sysfs_attributes.name ); \
} \
DRIVER_ATTR(name, (0660), name##_show, NULL);


//attributs in readonly
MY_DRIVER_ATTR_RO(buffer_base_phys0);
MY_DRIVER_ATTR_RO(buffer_base_phys1);
MY_DRIVER_ATTR_RO(buffer_base_phys2);



struct driver_attribute *driver_attribute_array[3] = {

	&driver_attr_buffer_base_phys0,
	&driver_attr_buffer_base_phys1,
	&driver_attr_buffer_base_phys2
};

struct platform_driver my_platform_driver = {
	.driver = {
		.name = "myVDMA",
		.owner = THIS_MODULE,
	}
};

static int __init mymodule_init(void)
{
	int ret_val;
	int i;

	pr_info ("mymodule loading...\n");

	my_kernel_buffer0 = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if(my_kernel_buffer0 == NULL) {
		pr_err("could not allocate buffer0\n");
		return(-ENOMEM);
	}
	my_kernel_buffer1 = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if(my_kernel_buffer1 == NULL) {
		pr_err("could not allocate buffer1\n");
		return(-ENOMEM);
	}
	my_kernel_buffer2 = kmalloc(BUFFER_SIZE, GFP_KERNEL);
		if(my_kernel_buffer2 == NULL) {
			pr_err("could not allocate buffer2\n");
			return(-ENOMEM);
		}


	//ASCII addresses sep virgules
	the_my_sysfs_attributes.buffer_base_phys0 = (uint32_t)(virt_to_phys(my_kernel_buffer0));
	the_my_sysfs_attributes.buffer_base_phys1 = (uint32_t)(virt_to_phys(my_kernel_buffer1));
	the_my_sysfs_attributes.buffer_base_phys2 = (uint32_t)(virt_to_phys(my_kernel_buffer2));


	ret_val = platform_driver_register(&my_platform_driver);
	if(ret_val != 0) {
		pr_err("platform_driver_register returned %d\n", ret_val);
		return(ret_val);
	}

	//creates file (attributs)
	for(i = 0 ; i < sizeof driver_attribute_array/sizeof *driver_attribute_array ; i++ ) {
		ret_val = driver_create_file(&my_platform_driver.driver, driver_attribute_array[i]);
		if(ret_val != 0) {
			pr_err("driver_create_file returned %d\n", ret_val);
			return(ret_val);
		}
	}
        return 0;
}

static void __exit mymodule_exit(void)
{
	int i;

	pr_info ("mymodule unloading...\n");
	for(i = 0 ; i < sizeof driver_attribute_array/sizeof *driver_attribute_array ; i++ ) {
		driver_remove_file(&my_platform_driver.driver, driver_attribute_array[i]);
	}

	platform_driver_unregister(&my_platform_driver);

	kfree(my_kernel_buffer0);
	kfree(my_kernel_buffer1);
	kfree(my_kernel_buffer2);
	return;
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
