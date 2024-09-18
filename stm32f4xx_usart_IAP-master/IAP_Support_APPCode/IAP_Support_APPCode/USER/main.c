#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "iap.h"
#include "stmflash.h"

//ALIENTEK ̽����STM32F407������ ʵ��56
//UCOSIIʵ��1-������� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO       			7 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);


//LED1����
//�����������ȼ�
#define LED1_TASK_PRIO       			6 
//���������ջ��С
#define LED1_STK_SIZE  					64
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//USART��������
//�����������ȼ�
#define USART_TASK_PRIO       			5 
//���������ջ��С
#define USART_STK_SIZE  				128
//�����ջ
OS_STK USART_TASK_STK[USART_STK_SIZE];
//������
void USART_Handle_TASK(void *pdata);

int main(void)
{ 
 
	delay_init(168);		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿� 
	uart_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����ϵͳ�ж����ȼ�����2
	
	SCB->VTOR=FLASH_BASE|0x4000;
	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//������ʼ����
	OSStart();	
}

 //��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(USART_Handle_TASK,(void *)0,(OS_STK*)&USART_TASK_STK[USART_STK_SIZE-1],USART_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 

//LED0����
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

//LED1����
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

//���ڴ�������
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
			delay_ms(10);   //10ms��û�н��յ������ݣ��ж�Ϊ1֡�Ѿ��������
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
			if(((*(vu32*)(STM32_FLASH_BASE + 4)) & 0xFF000000) == 0x08000000)		//�жϴ���Ϸ���
			{
				STMFLASH_Write(ADDR_CodeWriteFlag,IAPFlagBuf,2);					//дIAP��ת��־λ
				delay_ms(100);
				printf("Start Running IAP code!\r\n");
				NVIC_SystemReset();				 //ע��˴���ת����ʹ��iap_load_app�����ǽ�ϵͳ��λ
			}
			else 
			{
				printf("IAP code is illegal!\r\n");  
			}
		}
		delay_ms(500);	//whileѭ��
	}
}
