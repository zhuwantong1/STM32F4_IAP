#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h" 
#include "flash.h" 
#include "iap.h"   

int main(void)
{ 
	u32 RxDataCount = 0;								//���ڽ��յ������ݼ���
	u32 AppCodeLength = 0;								//���յ���app���볤��
	u32 RxDataLength = 0;
	u8  RxCmdFlag = 0;
	u8  AppRunFlag = 0;									//Ӧ�ó������б�־
	u32 JumpToAPPFlag;									//��ת��APP�����־λ����λ��1�����Ѿ�д��APP����
	u32 JumpToIAPFlag;									//��ת��IAP��־λ
	u32 IAPFlagBuf[2];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//����ϵͳ�ж����ȼ�����2
	delay_init(168);  									//��ʼ����ʱ����
	LED_Init();											//��ʼ��LED 
	uart_init(115200);									//��ʼ�����ڲ�����Ϊ115200
	JumpToAPPFlag = FLASH_ReadWord(ADDR_CodeWriteFlag); //��ȡAPPд���־λ���ж��Ƿ����г���
	JumpToIAPFlag = FLASH_ReadWord(ADDR_JumpToIAPFlag); //��ȡ��־λ���ж��Ƿ��APP��ת����
	IAPFlagBuf[0] = 0x55;								//IAP��ת��APP����־λ�жϵ�ַд��0x55,0x00,APP��ת��IAP����־λ�жϵ�ַд��0x00,0x55
	IAPFlagBuf[1] = 0x00;
	
	while(1)
	{
		if(JumpToAPPFlag != 0x55)			//�ж��Ƿ�����APP����������У���ת��APP��������
		{
			if(JumpToIAPFlag == 0x55)		//�ж��Ƿ��APP������ת����,����ǲ�������APP����
			{
				STMFLASH_Erase();			
				JumpToIAPFlag = 0x00;		//����ת��־λ,��ֹ��ͣ����FLASH
			}
			if(USART_RX_CNT)
			{
				if(RxDataCount == USART_RX_CNT)				//����û�����յ�������
				{
					RxDataLength = USART_RX_CNT;
					if(RxCmdFlag == 0 && RxDataLength == 5)	//����IAPָ��
					{
						if(USART_RX_BUF[0] == 0xA5 && USART_RX_BUF[1] == 0x5A && USART_RX_BUF[2] == 0x31 
													&& USART_RX_BUF[3] == 0x42 && USART_RX_BUF[4] == 0x0D)
						{
							RxCmdFlag = 1;					//���յ�����APP����ָ���־λ��λ
							RxDataLength = 0;				//���ָ��ȣ���ֹӰ��������APP�����С
							printf("Ready to recieve app code,please add a binary file!\r\n"); //׼���ý���bin�ļ����ȴ��û����
						}
						else
						{
							CodeUpdateFlag = 0;
							AppCodeLength = 0;
							printf("Command Error!\r\n");	//δ���յ�IAP����ָ������κδ��ڷ������ݶ���Ϊָ�����
						}
					}
					
					else if(RxCmdFlag == 1 && RxDataLength > 10)//����IAP����
					{
						CodeUpdateFlag = 1;					//������±�־λ��λ������Ӧ�ó�����������ɺ�дFLASH
						RxCmdFlag = 0;
						AppCodeLength = USART_RX_CNT;
						printf("App code recieve complete!\r\n");
						printf("Code size:%dBytes\r\n",AppCodeLength);
					}
					
					else
					{
						RxDataLength = 0;
						printf("Not correct file or command!\r\n"); //��������С����10Bytes����Ϊû����ȷ���bin�ļ�
					}
					RxDataCount = 0;
					USART_RX_CNT = 0;
				}
				else 
				{
					RxDataCount = USART_RX_CNT;
				}
			}
			
			delay_ms(10);									//���Դ����жϵ�ʱ�䣬�ж��Ƿ�������
			
			if(CodeUpdateFlag)								//������±�־λ��λ
			{
				CodeUpdateFlag = 0;
				if(AppCodeLength)
				{
					printf("Updating app code...\r\n");
					if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)					//�жϴ���Ϸ���
					{	 
						IAP_Write_Appbin(FLASH_APPCODE_ADDR,USART_RX_BUF,AppCodeLength);	//�´���д��FLASH  
						printf("Code Update Complete!Jump to App Code In 5 second!\r\n");
						delay_ms(1000);
						printf("Code Update Complete!Jump to App Code In 4 second!\r\n");
						delay_ms(1000);
						printf("Code Update Complete!Jump to App Code In 3 second!\r\n");
						delay_ms(1000);
						printf("Code Update Complete!Jump to App Code In 2 second!\r\n");
						delay_ms(1000);
						printf("Code Update Complete!Jump to App Code In 1 second!\r\n");
						delay_ms(1000);
						AppRunFlag = 1;
					}
					else 
					{
						printf("Code update failed!Please check legality of the binary file!\r\n");
					}
				}
				else 
				{
					printf("No Code Can Update!\r\n");
				}								 
			}
			
			if(AppRunFlag)																//App���б�־��λ
			{
				printf("Start running app code!\r\n");
				delay_ms(10);
				if(((*(vu32*)(FLASH_APPCODE_ADDR + 4)) & 0xFF000000) == 0x08000000)		//�жϴ���Ϸ���
				{	 
					AppRunFlag = 0;
					FLASH_WriteWord(ADDR_CodeWriteFlag,IAPFlagBuf,2);					//д��APP�����־
					IAP_Load_AppCode(FLASH_APPCODE_ADDR);								//ִ��FLASH APP����
				}
				else 
				{
					printf("App code is illegal!\r\n");  
				}									   
			}
		}
		else
		{
			printf("There is an app code!\r\n");
			printf("Start running app code!\r\n");
			delay_ms(10);
			IAP_Load_AppCode(FLASH_APPCODE_ADDR);								//ִ��FLASH APP����
		}
	}  
}

