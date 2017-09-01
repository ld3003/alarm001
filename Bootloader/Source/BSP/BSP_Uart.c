#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <misc.h>

#include "BSP_Uart.h"
#include "atcmd.h"

CirQueue UartRingQueue;

void BSP_UART1Config(int baud)
{
	NVIC_InitTypeDef		NVIC_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	GPIO_InitTypeDef		GPIO_InitStructure;
	
	//初始化环形缓冲
	InitQueue(&UartRingQueue);

	//RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//??UASRT???
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//??GPIOA???,?????,??????,????,???????????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//GPIO
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; //?????,IO???????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //??????;??????????GPIO_Mode_Out_PP????????,??????,?????????;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //???????,????????????IO?????
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//USART1
	USART_InitStructure.USART_BaudRate= baud;     //?????
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //?????
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //???
	USART_InitStructure.USART_Parity= USART_Parity_No ;         //?????
	 
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None; //???
	 
	USART_InitStructure.USART_Mode =USART_Mode_Rx |USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	 
	USART_Cmd(USART1, ENABLE);
	
	//NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      //????2
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //??USART????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //?????
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;          //????
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //??????
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    //????????	 
	USART_Cmd(USART1, ENABLE);


	
	//
}

void UART1_Write_buffer(unsigned char * buffer, int length)
{
	int i=0;
	for(i=0;i<length;i++)
	{
		
		USART_SendData(USART1, buffer[i]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
		
		//
	}
	//
}


#include <stm32f10x_usart.h>
void USART1_IRQHandler(void)
{
	unsigned char tmp = 0x0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		tmp = USART_ReceiveData(USART1);
		//recv
		//EnQueue(&UartRingQueue,tmp); //放入队列
		
	}
	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(USART1, USART_IT_TC);
	}

	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}

	if (USART_GetITStatus(USART1, USART_IT_CTS) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_CTS);
	}

	if (USART_GetITStatus(USART1, USART_IT_LBD) != RESET)
	{  
		USART_ClearITPendingBit(USART1, USART_IT_LBD);
	}

	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)    //??????
	{
		(u16)(USART1->DR & (u16)0x01FF);   
	}

}
