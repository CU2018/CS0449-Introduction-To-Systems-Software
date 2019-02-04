//Siyu Zhang
//siz24
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>

#include <asm/uaccess.h>

static ssize_t dice_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)  //return 0 when failed
{
	if (count <= 0)  //the num of rolls that the user asks for is an error value
		return -EINVAL;
	
	//dynamically-sized buffer (stack space in kernel is limited)
	unsigned char* mybuf;
	mybuf = kmalloc(count, GFP_KERNEL); 
	if (!mybuf)  //the allocation failed
	{
		printk(KERN_ERR "The allocation of mybuf failed!\n");
		return -EINVAL;
	}
		
	
	get_random_bytes(mybuf, count);
	 //loop over the array and turn each byte into a random number in the range 0-5
	int i;
	for (i = 0; i < count; i++) 
	{
		mybuf[i] %= 6;
	}
	
	
	//copy_to_user: Returns number of bytes that could not be copied. On success, this will be zero.
	if (copy_to_user(buf, mybuf, count))
	{
		kfree(mybuf);  //free the allocated buffer
		return -EINVAL;  //return a negative number if failed
	}
	
	*ppos = *ppos + count;  //tell the OS we've moved count bytes through the file
	
	kfree(mybuf);  //free the allocated buffer
	return count;  //how many dice rolls the user wants
}

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_dev = {
	MISC_DYNAMIC_MINOR,
	
	"dice",
	
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	ret = misc_register(&dice_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"dice_dev\" misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Siyu Zhang <siz24@pitt.edu>");
MODULE_DESCRIPTION("\"Dice Driver\" minimal module");
MODULE_VERSION("dev");
