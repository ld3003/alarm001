/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The USART application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bsp.h"
#include <stdio.h>
#include <stdlib.h>
#include "utimer.h"
#include "task.h"
#include "mem.h"
#include "serialport.h"
#include "mainloop.h"
#include "bsp.h"
#include "config.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


static void test_timer(void *arg)
{
	printf("current RTC time %d \r\n",CURRENT_RTC_TIM);
}

static void test_mma(void)
{
	
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
int main(void)
{
	
	unsigned short Data2,Data1;
	
	SysTick_Config(SystemCoreClock / 100);
	init_uart1();
	init_uart2();
	//RTC_Init();
	
	init_utimer();
	init_task();
	init_mem();
	init_uart2_buffer();
	
	IOI2C_Init();

	
	init_mma845x();
	utimer_sleep(5000);
	Sys_Enter_Standby();
	for(;;){};
	
	JpegBuffer = malloc(1025 * 10);
	ov_poweron();
	config_mco();
	InitSCCB();
	__test_ov2640();
	
	for(;;){readimg();};
	
	return 1;
	
	//
	//Data1 = *(unsigned short *)(0X1FFFF7BA);
	//printf("DATA1 DADA2 %X %X \r\n",Data1,Data2);
	
	
	mem = (struct MEM_DATA *)malloc(sizeof(struct MEM_DATA));
	read_config();
	
	
	
	//启动一个定时器，进行喂狗操作
	timerlist[0].func = test_timer;
	timerlist[0].arg = 0;
	timerlist[0].type = 0;
	timerlist[0].__dangqian = 0;
	timerlist[0].__zhouqi = 10;
	timerlist[0].enable = 1;
	
	for(;;)
	{
		run_task();
		mainloop();
	}
	
	while (1){
		/* Loop until RXNE = 1 */
				while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		USART_SendData(USART1,USART_ReceiveData(USART1));
	}
}





/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

