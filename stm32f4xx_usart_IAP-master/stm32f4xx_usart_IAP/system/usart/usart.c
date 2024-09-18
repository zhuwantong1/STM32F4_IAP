#include "sys.h"
#include "usart.h"

//ucos使用以下代码
#if SYSTEM_SUPPORT_OS
#include "includes.h"					
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0X20001000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.    

u16 USART_RX_STA = 0;       //接收状态标记	  
u32 USART_RX_CNT = 0;		//接收的字节数
u8  CodeUpdateFlag = 0;		//App代码更新标志
  
/*串口初始化*/
void uart_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 	//PA9和PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;					//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);				//开启接收中断

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//串口1中断入口
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
#endif

}

/*串口1中断服务程序*/
void USART1_IRQHandler(void)                	
{
	u8 Res;
#if SYSTEM_SUPPORT_OS
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART1);
		if(USART_RX_CNT < USART_REC_LEN)
		{
			USART_RX_BUF[USART_RX_CNT] = Res;
			USART_RX_CNT++;			 									     
		}	 	
	} 
#if SYSTEM_SUPPORT_OS
	OSIntExit();											 
#endif
}

 



