#ifndef _FLASH_H
#define _FLASH_H
#include "sys.h"
/******************************************************************************
* File       : flash.h
* Function   : FLASH Head File
* Description: None          
* Version    : V1.00
* Author     : LC
* Date       : 
* History    :  
* No.					|Date						|Author					|Description
   1					|2020-10-27					|LC					    |V1.0
******************************************************************************/
#define STM32_FLASH_BASE   		0x08000000			//STM32 FLASH��ʼ��ַ
#define STM32_FLASH_SIZE		1024				//1024k Bytes

#define ADDR_FLASH_SECTOR0		((u32)0x08000000)			//SECTOR0 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR1		((u32)0x08004000)			//SECTOR1 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR2		((u32)0x08008000)			//SECTOR2 FLASH��ʼ��ַ,���ڴ洢����ı���ʹ������
#define ADDR_FLASH_SECTOR3		((u32)0x0800C000)			//SECTOR3 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR4		((u32)0x08010000)			//SECTOR4 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR5		((u32)0x08020000)			//SECTOR5 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR6		((u32)0x08040000)			//SECTOR6 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR7		((u32)0x08060000)			//SECTOR7 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR8		((u32)0x08080000)			//SECTOR8 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR9		((u32)0x080A0000)			//SECTOR9 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR10		((u32)0x080C0000)			//SECTOR10 FLASH��ʼ��ַ
#define ADDR_FLASH_SECTOR11		((u32)0x080E0000)			//SECTOR11 FLASH��ʼ��ַ
#define ADDR_CodeWriteFlag      ((u32)0x080E1000)			//IAP������±�־λ��ŵ�ַ�������0x55,˵���Ѿ���Ӧ�ó���
#define ADDR_JumpToIAPFlag		((u32)0x080E1001)			//IAP������ת��־λ�������0x55,˵���Ǵ�APP��ת��IAP����

#define ERROR			0
#define COMPLETE		1


u32 FLASH_ReadWord(u32 faddr);
uint16_t GET_FLASH_SECTOR(u32 addr);
u8 FLASH_WriteWord(u32 addr,u32 *pbuffer,u32 DataNum);
void WriteParaToFlash(u32 addr,u32 DataNum);
u8 EraseParaInFlash(u32 addr);
void ReadParaFromFlash(u32 addr);
u8 STMFLASH_Erase(void);


#endif
