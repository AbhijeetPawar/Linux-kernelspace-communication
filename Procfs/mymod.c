#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");


#define MAX_BUFFER 1024

static struct proc_dir_entry *proc_entry;

static char* BUFFER;

static int read_index;
static int write_index;


static int my_write( struct file *filp, const char __user *buff,

                        unsigned long len, void *data )
                        
{
	int space_available = MAX_BUFFER - write_index;
	
	if(len > space_available)
	{
		printk(KERN_INFO "BUFFER is FULL !");
		return -ENOSPC;
	}
	
	if(copy_from_user(&BUFFER[write_index],buff,len))
		return -EFAULT;

	write_index += len;
	
	BUFFER[write_index-1] = '\0';
	
	printk(KERN_INFO "my_write called !");
	
	return len;
}


static int my_read( char *page, char **start, off_t off,

                   int count, int *eof, void *data )
                   
{
	int len;
	
	if(off > 0)
	{
		*eof = 1;
		return 0;
	}
	
	if(read_index >= write_index)
		read_index = 0;	
		
	len = sprintf(page,"%s\n",&BUFFER[read_index]);

	read_index += len;

	printk(KERN_INFO "my_read called !");
	
	return len;
}


static int mod_init()
{
	
	BUFFER = (char *)vmalloc(MAX_BUFFER);
	
	if(!BUFFER)
	{
		return -ENOMEM;
	}	
	else
	{
		proc_entry = create_proc_entry("kfile",0644,NULL);	//name,mode,parent
		
		if(proc_entry==NULL)
		{
			vfree(BUFFER);
			printk(KERN_INFO "Couldn't create proc_entry !");
			return -ENOMEM;
		}
		else
		{
			read_index = write_index = 0;
			
			proc_entry->read_proc = my_read;
			
			proc_entry->write_proc = my_write;		
			
			printk(KERN_INFO "KFILE : Module Loaded Successfully ! ");
		}
	}
	return 0;
}


static void mod_exit()
{
	remove_proc_entry("kfile", NULL);
	vfree(BUFFER);
	printk(KERN_INFO "KFILE : Module Unloaded ! ");
}

module_init(mod_init);
module_exit(mod_exit);




