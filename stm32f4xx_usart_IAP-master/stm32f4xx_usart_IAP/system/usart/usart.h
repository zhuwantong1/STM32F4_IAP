#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART_REC_LEN  			112*1024 //240K�ֽ�APP���д���
//�����������ֽ���:SECTOR1 16K + SECTOR2 16K + SECTOR3 16K + SECTOR4 64K,���ڴ��APP���д��룬SECTOR5-SECTOR11���ڴ��ϵͳ����

#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern u32 USART_RX_CNT;				//���յ��ֽ���	
extern u8  CodeUpdateFlag;

void uart_init(u32 bound);
#endif


