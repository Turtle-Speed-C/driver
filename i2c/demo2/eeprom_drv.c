#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/io.h>
#include <linux/gpio.h>
#include <cfg_type.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>

struct i2c_client *gec6818_eeprom_client;

static int gec6818_eeprom_open(struct inode *inode, struct file *file){
	printk(KERN_INFO "gec6818_eeprom_open called\n");
	return 0;
}

static int gec6818_eeprom_release(struct inode *inode, struct file *file){
	printk(KERN_INFO "gec6818_eeprom_release called\n");
	return 0;
}

static ssize_t gec6818_eeprom_read(struct file *file, char __user *buf, size_t count, loff_t *offset){


}

static ssize_t gec6818_eeprom_write(struct file *file, const char __user *buf, size_t count, loff_t *offset){
	int ret;

	char wbuf[16];
	char addr=0;
	struct i2c_msg msg[2];

	if(len>sizeof(wbuf)){
		len=sizeof(wbuf);
	}

	ret=copy_from_user(wbuf, buf, len);
	if(ret < 0){
		printk(KERN_ERR "copy_from_user failed\n");
		return -EFAULT;
	}

	msg[0].addr = gec6818_eeprom_client->addr;//从机地址
	msg[0].flags = 0; // 写操作
	msg[0].len = 1; // 地址长度
	msg[0].buf = &addr; // 地址缓冲区

	msg[1].addr = gec6818_eeprom_client->addr;//从机地址
	msg[1].flags = 0|I2C_M_NOSTART; // 写操作
	msg[1].len = len; // 数据长度
	msg[1].buf = wbuf; // 数据缓冲区

	ret=i2c_stransfer(gec6818_eeprom_client->adapter, msg, 2);
	if(ret < 0){
		printk(KERN_ERR "i2c_transfer failed\n");
		return ret;
	}
	
}

static const struct file_operations gec6818_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_eeprom_open,
	.release = gec6818_eeprom_release,
	.read = gec6818_eeprom_read,
	.write = gec6818_eeprom_write,
};

static struct miscdevice gec6818_eeprom_miscdev={
	.minor=MISC_DYNAMIC_MINOR,
	.name="gec6818_eeprom",
	.fops=&gec6818_fops,
};

void gec6818_eeprom_probe(struct i2c_client *client, const struct i2c_device_id *id){
	int ret;
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_ERR "I2C functionality check failed\n");
		return;
	}

	// 注册misc设备
	ret=misc_register(&gec6818_eeprom_miscdev);
	if (ret < 0) {
		printk(KERN_ERR "Failed to register misc device\n");
		return;
	}

	gec6818_eeprom_client = client;

	printk(KERN_INFO "gec6818_eeprom_probe called\n");
	// 设备初始化代码
}

static __devexit int gec6818_eeprom_remove(struct i2c_client* client){

	misc_unregister(&gec6818_eeprom_miscdev);

	return 0;
}

static const struct i2c_device_id gec6818_eeprom_id[]={
	{"gec6818_eeprom", 0},
	{}
}

MODULE_DEVCE_TABLE(i2c, gec6818_eeprom_id); // 设备ID表

//1.i2c_device_id：驱动在代码里声明"我能处理这些设备"
//2.MODULE_DEVICE_TABLE：向内核注册"这些设备请找我来处理"
//3.自动匹配：当设备插入时，内核自动匹配驱动和设备
static struct i2c_driver gec6818_eeprom_driver={
	.driver={
		.name="gec6818_eeprom",
		.owner=THIS_MODULE,
	},
	.probe=gec6818_eeprom_probe,
	.remove=__devexit_p(gec6818_eeprom_remove),
	.id_table=gec6818_eeprom_id,		// 设备ID表

};

//函数入口
static int gec6818_eeprom_drv_init(void){
	int ret;
	ret=i2c_add_driver(&gec6818_eeprom_driver);
	return ret;
}

//函数出口
static void gec6818_eeprom_drv_exit(void){
	i2c_del_driver(&gec6818_eeprom_driver);
}

module_init(gec6818_eeprom_drv_init);
module_exit(gec6818_eeprom_drv_exit);

//模块描述
MODULE_AUTHOR("LI XIAOFAN");			//作者信息
MODULE_DESCRIPTION("gec6818 eeprom driver");	//模块功能说明
MODULE_LICENSE("GPL");							//许可证：驱动遵循GPL协议