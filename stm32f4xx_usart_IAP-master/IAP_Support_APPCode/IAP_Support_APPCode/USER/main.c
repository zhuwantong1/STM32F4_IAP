#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "iap.h"
#include "stmflash.h"

//ALIENTEK 探索者STM32F407开发板 实验56
//UCOSII实验1-任务调度 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO       			7 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);


//LED1任务
//设置任务优先级
#define LED1_TASK_PRIO       			6 
//设置任务堆栈大小
#define LED1_STK_SIZE  					64
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//USART处理任务
//设置任务优先级
#define USART_TASK_PRIO       			5 
//设置任务堆栈大小
#define USART_STK_SIZE  				128
//任务堆栈
OS_STK USART_TASK_STK[USART_STK_SIZE];
//任务函数
void USART_Handle_TASK(void *pdata);

int main(void)
{ 
 
	delay_init(168);		  //初始化延时函数
	LED_Init();		        //初始化LED端口 
	uart_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置系统中断优先级分组2
	
	SCB->VTOR=FLASH_BASE|0x4000;
	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//创建起始任务
	OSStart();	
}

 //开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(USART_Handle_TASK,(void *)0,(OS_STK*)&USART_TASK_STK[USART_STK_SIZE-1],USART_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 

//LED0任务
void led0_task(void *pdata)
{	 	
	while(1)
	{
		LED0=0;
		delay_ms(80);
		LED0=1;
		delay_ms(920);
	};
}

//LED1任务
void led1_task(void *pdata)
{
	while(1)
	{
		LED1=0;
		delay_ms(300);
		LED1=1;
		delay_ms(300);
	}
}

//串口处理任务
void USART_Handle_TASK(void *pdata)
{
	u8 USART_CNT;
	u8 Jump_IAP_FLAG = 0;
	u32 IAPFlagBuf[2];
	IAPFlagBuf[0] = 0x00;
	IAPFlagBuf[1] = 0x55;
	while(1)
	{
		if(USART_RX_STA)
		{
			USART_RX_STA = 0;
			USART_CNT = USART_RX_CNT;
			delay_ms(10);   //10ms内没有接收到新数据，判断为1帧已经发送完成
			if(USART_RX_CNT == USART_CNT)
			{
				USART_RX_Length = USART_RX_CNT;
				USART_RX_CNT = 0;
				if(USART_RX_BUF[0] == 0xA5 && USART_RX_BUF[1] == 0x5A && USART_RX_BUF[2] == 0x31 
											&& USART_RX_BUF[3] == 0x53 && USART_RX_BUF[4] == 0x0D)
				{
					Jump_IAP_FLAG = 1;
					printf("Jump Back To IAP Program!\r\n");
				}
			}
		}
		if(Jump_IAP_FLAG)
		{
			Jump_IAP_FLAG = 0;
			delay_ms(10);
			if(((*(vu32*)(STM32_FLASH_BASE + 4)) & 0xFF000000) == 0x08000000)		//判断代码合法性
			{
				STMFLASH_Write(ADDR_CodeWriteFlag,IAPFlagBuf,2);					//写IAP跳转标志位
				delay_ms(100);
				printf("Start Running IAP code!\r\n");
				NVIC_SystemReset();				 //注意此处跳转不是使用iap_load_app，而是将系统复位
			}
			else 
			{
				printf("IAP code is illegal!\r\n");  
			}
		}
		delay_ms(500);	//while循环
	}
}
