#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h" 
#include "flash.h" 
#include "iap.h"   

int main(void)
{ 
	u32 RxDataCount = 0;								//串口接收到的数据计数
	u32 AppCodeLength = 0;								//接收到的app代码长度
	u32 RxDataLength = 0;
	u8  RxCmdFlag = 0;
	u8  AppRunFlag = 0;									//应用程序运行标志
	u32 JumpToAPPFlag;									//跳转至APP程序标志位，此位置1代表已经写入APP代码
	u32 JumpToIAPFlag;									//跳转回IAP标志位
	u32 IAPFlagBuf[2];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//设置系统中断优先级分组2
	delay_init(168);  									//初始化延时函数
	LED_Init();											//初始化LED 
	uart_init(115200);									//初始化串口波特率为115200
	JumpToAPPFlag = FLASH_ReadWord(ADDR_CodeWriteFlag); //读取APP写入标志位，判断是否已有程序
	JumpToIAPFlag = FLASH_ReadWord(ADDR_JumpToIAPFlag); //读取标志位，判断是否从APP跳转回来
	IAPFlagBuf[0] = 0x55;								//IAP跳转至APP，标志位判断地址写入0x55,0x00,APP跳转至IAP，标志位判断地址写入0x00,0x55
	IAPFlagBuf[1] = 0x00;
	
	while(1)
	{
		if(JumpToAPPFlag != 0x55)			//判断是否已有APP程序，如果已有，跳转至APP程序运行
		{
			if(JumpToIAPFlag == 0x55)		//判断是否从APP程序跳转回来,如果是擦除已有APP程序
			{
				STMFLASH_Erase();			
				JumpToIAPFlag = 0x00;		//清跳转标志位,防止不停擦除FLASH
			}
			if(USART_RX_CNT)
			{
				if(RxDataCount == USART_RX_CNT)				//串口没有再收到新数据
				{
					RxDataLength = USART_RX_CNT;
					if(RxCmdFlag == 0 && RxDataLength == 5)	//接收IAP指令
					{
						if(USART_RX_BUF[0] == 0xA5 && USART_RX_BUF[1] == 0x5A && USART_RX_BUF[2] == 0x31 
													&& USART_RX_BUF[3] == 0x42 && USART_RX_BUF[4] == 0x0D)
						{
							RxCmdFlag = 1;					//接收到更新APP代码指令，标志位置位
							RxDataLength = 0;				//清空指令长度，防止影响后面计算APP代码大小
							printf("Ready to recieve app code,please add a binary file!\r\n"); //准备好接收bin文件，等待用户添加
						}
						else
						{
							CodeUpdateFlag = 0;
							AppCodeLength = 0;
							printf("Command Error!\r\n");	//未接收到IAP更新指令，其他任何串口发送数据都认为指令错误
						}
					}
					
					else if(RxCmdFlag == 1 && RxDataLength > 10)//接收IAP程序
					{
						CodeUpdateFlag = 1;					//代码更新标志位置位，用于应用程序代码接收完成后写FLASH
						RxCmdFlag = 0;
						AppCodeLength = USART_RX_CNT;
						printf("App code recieve complete!\r\n");
						printf("Code size:%dBytes\r\n",AppCodeLength);
					}
					
					else
					{
						RxDataLength = 0;
						printf("Not correct file or command!\r\n"); //如果代码大小不足10Bytes，认为没有正确添加bin文件
					}
					RxDataCount = 0;
					USART_RX_CNT = 0;
				}
				else 
				{
					RxDataCount = USART_RX_CNT;
				}
			}
			
			delay_ms(10);									//给以串口中断的时间，判断是否接收完成
			
			if(CodeUpdateFlag)								//代码更新标志位置位
			{
				CodeUpdateFlag = 0;
				if(AppCodeLength)
				{
					printf("Updating app code...\r\n");
					if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)					//判断代码合法性
					{	 
						IAP_Write_Appbin(FLASH_APPCODE_ADDR,USART_RX_BUF,AppCodeLength);	//新代码写入FLASH  
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
			
			if(AppRunFlag)																//App运行标志置位
			{
				printf("Start running app code!\r\n");
				delay_ms(10);
				if(((*(vu32*)(FLASH_APPCODE_ADDR + 4)) & 0xFF000000) == 0x08000000)		//判断代码合法性
				{	 
					AppRunFlag = 0;
					FLASH_WriteWord(ADDR_CodeWriteFlag,IAPFlagBuf,2);					//写入APP代码标志
					IAP_Load_AppCode(FLASH_APPCODE_ADDR);								//执行FLASH APP代码
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
			IAP_Load_AppCode(FLASH_APPCODE_ADDR);								//执行FLASH APP代码
		}
	}  
}

