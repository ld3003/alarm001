/*
********************************************************************************
*
*                                 APP.c
*
* File          : APP.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : 主程序入口
*                
* History       :
* Date          : 2013.07.21
*******************************************************************************/


#include <string.h>
#include <stdio.h>

#include "APP.h"
#include "BSP.h"
#include "BSP_tim.h"
#include "BSP_Uart.h"

#include "HW_Config.h"

#include "atcmd.h"

unsigned char fputcmod = 0;

#define SERIALPORT		fputcmod = 0;
#define USBPORT				fputcmod = 1;

static void process_atcmd(void);
static void process_usbdata(void);

void LedHandler(void)
{
    IO_TOGGLE(eLED_0);
}

void Led2Handler(void)
{
    IO_TOGGLE(eLED_0);
}


/*******************************************************************************
* Function Name : int main(void)
* Description   : 主程序入口
* Input         : 
* Output        : 
* Other         : 
* Date          : 2013.07.21
*******************************************************************************/
int main(void)
{
		SysTickInit();
    BSP_Init();
		BSP_UART1Config(115200);
    USB_Config();
	
		SERIALPORT;
		printf("系统初始化完成\r\n");
	
    while (1)
    {
			//处理USB 数据
			process_usbdata();
			//处理串口AT数据
			process_atcmd();	
    }
}

static void process_atcmd(void)
{
	//处理AT指令
	switch(status_recv)
	{
		case STATUS_RECV_FINISH:
			break;
		case STATUS_RECV_ERROR:
			atcmd_recv_reset(); //发生错误了要复位
			break;
		default:
			break;
	}
	//
}

static void process_usbdata(void)
{
	uint32_t len = 0;
	static uint8_t buf[256] = {0};
	
	//将串口1发送过来的额数据，通过USB转发出去
	if (QueueIsEmpty(&UartRingQueue) == 0)
	{
		uint8_t tmp = (uint8_t)DeQueue(&UartRingQueue);
		USB_TxWrite(&tmp, 1);
	}
	
	len = USB_RxRead(buf, sizeof(buf));
	
	if (len > 0)
	{
		unsigned short i=0;
		
		UART1_Write_buffer(buf,len);
		
		for(i=0;i<len;i++)
		{
			uart_recv_handle(buf[i]); //放入解析
		}
			
	}
	
}

int fputc(int ch, FILE *f)
{
	unsigned char tmp = (char)ch;
	if (fputcmod == 1)
		USB_TxWrite(&tmp,1);
	else
	{
		USART_SendData(USART1, tmp);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
	}
	return (ch);
}


