#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>

#define CHR_DEV_NAME 	"MyBufferedMem"
#define CHR_DEV_MAJOR	255  

struct kfifo fifo;

//default N buffer size
static int WRITE_BUFFER_SIZE = 8;        

//default M read size
static int READ_BUFFER_SIZE = 2;    

//initialization N, M when dd registering 
module_param(WRITE_BUFFER_SIZE, int, 0000);   
MODULE_PARM_DESC(WRITE_BUFFER_SIZE, "default kfifo buffer size is 8bytes");
module_param(READ_BUFFER_SIZE, int, 0000);
MODULE_PARM_DESC(READ_BUFFER_SIZE, "default read size is 2bytes");

int chr_open(struct inode *inode, struct file *filep)
{
	printk(KERN_INFO"MyBufferedMem opened\n");
	return	0;
}

//ssize_t == signed int == int 
ssize_t	chr_write(struct file *filep, const char *buf, size_t count, loff_t *f_ops)
{	
	int ret_fifo,ret_copy;
	int i = 0;
	unsigned char each;
	char *user_buffer;

	//for sending data from user to kernel
	user_buffer = kmalloc(count,GFP_KERNEL);
	if(user_buffer == NULL){
		printk(KERN_ERR"kmalloc failed..\n");
		kfree(user_buffer);
	}else{
		printk(KERN_INFO"kmalloc success..\n");	
	}
	
	ret_copy = copy_from_user(user_buffer,buf,count);
	if(ret_copy == 0){
		printk(KERN_INFO"copy from user success..\n");	
	}else{
		printk(KERN_ERR"copy from user failed..\n");
	}
	//fifo in 
	while(i < count){
		if(kfifo_is_full(&fifo)){
			kfifo_out(&fifo,&each,sizeof(each));		
		}
		each = user_buffer[i];
		ret_fifo = kfifo_in(&fifo,&each,sizeof(each));
		
		i++;		
	}
	if(ret_fifo>0){
		printk(KERN_INFO"success kfifo_write..\n");
	}else{
		printk(KERN_ERR"fail kfifo_write..\n");
	}
	return	count;
}

ssize_t	chr_read(struct file *filep, char *buf, size_t count, loff_t *f_ops){

	int ret_fifo, ret_copy;
	int i = 0;
	char *user_buffer;
	char val;

	user_buffer = kmalloc(READ_BUFFER_SIZE,GFP_KERNEL);
	if(user_buffer == NULL){
		printk(KERN_ERR"kmalloc failed..\n");
		kfree(user_buffer);
	}else{
		printk(KERN_INFO"kmalloc success..\n");	
	}
	//fifo out
	while(i < count){
		if(kfifo_is_empty(&fifo)){
			break;		
		}
		ret_fifo = kfifo_out(&fifo,&val,sizeof(val));
		user_buffer[i] = val;
		printk("read data:%c\n",val);
		i++;		
	}
	
	if(ret_fifo>0){
		printk(KERN_INFO"success kfifo_read..\n");
	}else{
		printk(KERN_ERR"fail kfifo_read..\n");
	}
	//for receiving data from kernel to user
	ret_copy = copy_to_user(buf,user_buffer,READ_BUFFER_SIZE);
	if(ret_copy == 0){
		printk(KERN_INFO"copy from user success..\n");
		kfree(user_buffer);	
	}else{
		printk(KERN_ERR"copy from user failed..\n");
	}
	return	count;
}

long	chr_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	int new_buffer_size, ret;
	switch (cmd) {
		case 0: 
			READ_BUFFER_SIZE = arg;
			printk("new read buffer size is : %d\n",READ_BUFFER_SIZE);
			break;
		case 1: 
			new_buffer_size = arg;
			kfifo_free(&fifo);
			ret = kfifo_alloc(&fifo,new_buffer_size,GFP_KERNEL);
			if(ret){
				printk(KERN_ERR"error kfifo_realloc..\n");
			}else{
				printk(KERN_INFO"success kfifo_realloc..\n");
			}

			printk("new buffer size is : %d\n",new_buffer_size);
			break;
	}
	return	0;
}

int	chr_release(struct inode *inode, struct file *filep)
{
	printk(KERN_INFO"MyBufferedMem released\n");	
	return	0;
}

struct file_operations chr_fops =
{
	.owner = THIS_MODULE,
	.write = chr_write,
	.read = chr_read,
	.open = chr_open,
	.release = chr_release,
	.unlocked_ioctl = chr_ioctl,
};

int pj_init(void)
{
	int ret;

	int registration;

	ret = kfifo_alloc(&fifo,WRITE_BUFFER_SIZE,GFP_KERNEL);
	if(ret){
		printk(KERN_ERR"error kfifo_alloc..\n");
	}else{
		printk(KERN_INFO"success kfifo_alloc..\n");
	}

	registration = register_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME, &chr_fops);
	printk("MyBufferedMem201713747 device driver registered\n");
	if (registration < 0)
		return registration;

	return	0;
}

void pj_exit(void)
{
	printk("MyBufferedMem201713747 device driver unregistered\n");
	kfifo_free(&fifo);
	unregister_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME);	
}

module_init(pj_init);
module_exit(pj_exit);

MODULE_LICENSE("GPL");	
