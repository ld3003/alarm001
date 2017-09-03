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

#include "BSP_tim.h"
#include "BSP_Uart.h"

#include "HW_Config.h"

#include "atcmd.h"
#include "bsp.h"
#include "mem.h"
#include "flash.h"
#include "rtc.h"
#include <stdlib.h>

unsigned char fputcmod = 0;

#define SERIALPORT		fputcmod = 0;
#define USBPORT				fputcmod = 1;

static void process_atcmd(void);
static void process_usbdata(void);


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

	/*systick init*/
	RCC_ClocksTypeDef rccClk = {0};
	RCC_GetClocksFreq(&rccClk);
	SysTick_Config(rccClk.HCLK_Frequency / 100);
	
	RTC_Init();
	
	init_mem();
	
	/*Uart1 INIT*/
	BSP_UART1Config(115200);
	
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\n*");
	printf("\r\nBootloader [%s %s]\r\n",__DATE__,__TIME__);
	printf("\r\n");
	printf("\r\n");
	

	

	
	if (read_usb_status() == 0)
	{
		printf("USB 线缆已经插入 !\r\n");
		//
	}else{
		printf("USB 线未插入 !\r\n");
		
		gotoApp();
		
	}

	/*
			读取 USB状态，判断是否进入bootloader模式

			判断标记位 
			如果APP正常启动，则jump
			如果APP未正常启动，则打开指示灯
	*/

	//printf("USB PIN STATUS %d %d \r\n",BSP_GpioRead(USB_DM_PIN),BSP_GpioRead(USB_DP_PIN));

	//for(;;){};

	USB_Config();

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

////////

void write_usb_buffer(unsigned char*buf , int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		EnQueue(&UartRingQueue,buf[i]);
	}
}


typedef  void (*pFunction)(void);
static void JumpToApp(u32 appAddr)				
{
	pFunction JumpToApplication;
	u32 JumpAddress;
	
	//appAddr = CPU2_FW_ADDRESS;
	
	JumpAddress = *(u32*) (appAddr + 4);
	JumpToApplication = (pFunction)JumpAddress;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(vu32*) appAddr);
  JumpToApplication();
}
void gotoApp(void)
{
	JumpToApp(APPLICATION_ADDRESS);
	//
}




