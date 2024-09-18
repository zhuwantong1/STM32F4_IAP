#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

typedef  void (*IAPFUNC)(void);				//����һ���������͵Ĳ���
//APPӦ�ó�����FLASH�е���ʼ��ַ������SECTOR1��ʼΪ�û�������ʼ��ַ��SECTOR0���ڴ��IAP�����ַ����СΪ16KB
#define FLASH_APPCODE_ADDR		0x08004000  

void IAP_Load_AppCode(u32 APP_ADDR);		//��תִ���û�����
void IAP_Write_Appbin(u32 APP_ADDR,u8 *APP_Buffer,u32 APP_LEN);	//��ָ����ַ��ʼ,д��bin�ļ�

#endif
