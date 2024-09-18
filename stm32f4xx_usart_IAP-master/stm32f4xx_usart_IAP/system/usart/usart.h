#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART_REC_LEN  			112*1024 //240K字节APP运行代码
//定义最大接收字节数:SECTOR1 16K + SECTOR2 16K + SECTOR3 16K + SECTOR4 64K,用于存放APP运行代码，SECTOR5-SECTOR11用于存放系统参数

#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern u32 USART_RX_CNT;				//接收的字节数	
extern u8  CodeUpdateFlag;

void uart_init(u32 bound);
#endif


