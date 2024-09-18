#include "sys.h"
#include "flash.h"
#include "iap.h" 

IAPFUNC Jump_To_APP; 
u32 IAP_Buffer[512]; 	//2K字节缓存  

/*用户代码写入FLASH*/
void IAP_Write_Appbin(u32 APP_ADDR,u8 *APP_Buffer,u32 APP_LEN)
{
	u32 t;
	u16 i = 0;
	u32 temp;
	u32 APP_Code_ADDR = APP_ADDR;//当前写入的地址
	u8 *BufPtr = APP_Buffer;
	for(t = 0;t < APP_LEN;t += 4)
	{						   
		temp = (u32)BufPtr[3] << 24;   
		temp |= (u32)BufPtr[2] << 16;    
		temp |= (u32)BufPtr[1] << 8;
		temp |= (u32)BufPtr[0];	  
		BufPtr += 4;
		
		IAP_Buffer[i++] = temp;	    
		if(i==512)
		{
			i=0; 
			FLASH_WriteWord(APP_Code_ADDR,IAP_Buffer,512);
			APP_Code_ADDR += 2048;//偏移2K字节
		}
	} 
	if(i)FLASH_WriteWord(APP_Code_ADDR,IAP_Buffer,i);	//将剩余的一些内容字节写进去
}

/*跳转至运行APP代码*/
void IAP_Load_AppCode(u32 APP_ADDR)
{
	if(((*(vu32*)APP_ADDR) & 0x2FFE0000) == 0x20000000)	//检查栈顶地址是否合法.
	{ 
		Jump_To_APP = (IAPFUNC)*(vu32*)(APP_ADDR + 4);	//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)APP_ADDR);						//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		Jump_To_APP();									//跳转到APP
	}
}		 
