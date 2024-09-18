#include "sys.h"
#include "flash.h"
#include "iap.h" 

IAPFUNC Jump_To_APP; 
u32 IAP_Buffer[512]; 	//2K�ֽڻ���  

/*�û�����д��FLASH*/
void IAP_Write_Appbin(u32 APP_ADDR,u8 *APP_Buffer,u32 APP_LEN)
{
	u32 t;
	u16 i = 0;
	u32 temp;
	u32 APP_Code_ADDR = APP_ADDR;//��ǰд��ĵ�ַ
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
			APP_Code_ADDR += 2048;//ƫ��2K�ֽ�
		}
	} 
	if(i)FLASH_WriteWord(APP_Code_ADDR,IAP_Buffer,i);	//��ʣ���һЩ�����ֽ�д��ȥ
}

/*��ת������APP����*/
void IAP_Load_AppCode(u32 APP_ADDR)
{
	if(((*(vu32*)APP_ADDR) & 0x2FFE0000) == 0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		Jump_To_APP = (IAPFUNC)*(vu32*)(APP_ADDR + 4);	//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)APP_ADDR);						//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		Jump_To_APP();									//��ת��APP
	}
}		 
