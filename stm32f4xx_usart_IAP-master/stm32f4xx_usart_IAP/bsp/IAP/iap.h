#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

typedef  void (*IAPFUNC)(void);				//定义一个函数类型的参数
//APP应用程序在FLASH中的起始地址，即从SECTOR1开始为用户代码起始地址，SECTOR0用于存放IAP程序地址，大小为16KB
#define FLASH_APPCODE_ADDR		0x08004000  

void IAP_Load_AppCode(u32 APP_ADDR);		//跳转执行用户代码
void IAP_Write_Appbin(u32 APP_ADDR,u8 *APP_Buffer,u32 APP_LEN);	//在指定地址开始,写入bin文件

#endif
