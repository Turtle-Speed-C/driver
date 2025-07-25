#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

static unsigned char *p=NULL; 	//用来存储动态分配的内存地址
static unsigned long addr;

static int __init myled_init(void){
	printk(KERN_INFO"myled_init\n");
	
	addr=__get_free_pages(GFP_KERNEL,3);
	
	p=(unsigned char *)addr;	//动态分配64字节的内存
	/*
	GFP_KERNEL：表示可以阻塞并且用于内核内存分配。
	GFP_ATOMIC：表示分配内存时不能阻塞，适用于中断上下文。
	GFP_DMA：表示分配的内存可以用于 DMA 操作。
	*/
	if(p==NULL){
		printk(KERN_INFO"kmalloc fail\n");
		return -ENOMEM;			// 返回内存分配失败的错误码
	}
	
	strcpy(p,"This is lixiaofan");
	
	printk(KERN_INFO"p=%s\n",p);
	
	printk(KERN_INFO"p virt_addr:0x%p phy_addr:%lx\n",p,(unsigned long)virt_to_phys(p));
	//virt_to_phys(p)：将虚拟地址 p 转换为物理地址
	
	return 0;
}

static void __exit myled_exit(void){
	free_pages(addr,3);		 // 释放分配的内存
	printk("myled_exit\n");  // 打印卸载信息
}


module_init(myled_init);//驱动的入口，使用insmod命令加载该驱动
module_exit(myled_exit);//驱动的出口，使用rmmod命令卸载该驱动

MODULE_AUTHOR("LI XIAOFAN");
MODULE_DESCRIPTION("This is led driver");
MODULE_LICENSE("GPL");